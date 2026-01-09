#include "./header/utxo.h"
#include <sstream>
#include <iomanip>

// ------------------- Helpers -------------------

// Convert uint256 to hex string for keys / debug
static std::string toHex(const uint256& arr) {
    std::stringstream ss;
    for (auto b : arr) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return ss.str();
}

// Convert little-endian int to 4 bytes
static void writeLE32(uint32_t val, std::vector<uint8_t>& out) {
    out.push_back(val & 0xFF);
    out.push_back((val >> 8) & 0xFF);
    out.push_back((val >> 16) & 0xFF);
    out.push_back((val >> 24) & 0xFF);
}

// Convert little-endian int64 to 8 bytes
static void writeLE64(int64_t val, std::vector<uint8_t>& out) {
    for (int i = 0; i < 8; ++i)
        out.push_back((val >> (8 * i)) & 0xFF);
}

// ------------------- UTXOEntry serialization -------------------

std::vector<uint8_t> UTXOEntry::serialize() const {
    std::vector<uint8_t> out;

    // txId
    out.insert(out.end(), txId.begin(), txId.end());

    // index (little-endian)
    writeLE32(index, out);

    // scriptPubKey length + bytes
    writeLE32(static_cast<uint32_t>(scriptPubKey.size()), out);
    out.insert(out.end(), scriptPubKey.begin(), scriptPubKey.end());

    // value
    writeLE64(value, out);

    return out;
}

UTXOEntry UTXOEntry::deserialize(const std::vector<uint8_t>& data) {
    UTXOEntry u;
    size_t offset = 0;

    // txId
    std::copy(data.begin(), data.begin() + 32, u.txId.begin());
    offset += 32;

    // index
    u.index = data[offset] | (data[offset+1]<<8) | (data[offset+2]<<16) | (data[offset+3]<<24);
    offset += 4;

    // scriptPubKey length
    uint32_t scriptLen = data[offset] | (data[offset+1]<<8) | (data[offset+2]<<16) | (data[offset+3]<<24);
    offset += 4;

    // scriptPubKey bytes
    u.scriptPubKey.insert(u.scriptPubKey.end(), data.begin()+offset, data.begin()+offset+scriptLen);
    offset += scriptLen;

    // value
    u.value = 0;
    for (int i = 0; i < 8; ++i) u.value |= static_cast<int64_t>(data[offset + i]) << (8 * i);

    return u;
}

// ------------------- Helper for key -------------------
std::string UTXOSet::buildKey(const uint256& txId, uint32_t index) {
    return toHex(txId) + ":" + std::to_string(index);
}

// ------------------- UTXOSet methods -------------------
UTXOSet::UTXOSet(const std::string& dbPath) {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, dbPath, &db);
    if(!status.ok()) {
        std::cerr << "Failed to open RocksDB: " << status.ToString() << std::endl;
        db = nullptr;
    } else {
        std::cout << "RocksDB opened at: " << dbPath << std::endl;
    }
}

UTXOSet::~UTXOSet() {
    delete db;
}

void UTXOSet::addUTXO(const UTXOEntry& utxo) {
    std::string key = buildKey(utxo.txId, utxo.index);
    std::vector<uint8_t> value = utxo.serialize();
    rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, rocksdb::Slice(reinterpret_cast<const char*>(value.data()), value.size()));
    if(!s.ok()) std::cerr << "Add UTXO failed: " << s.ToString() << std::endl;
}

void UTXOSet::removeUTXO(const uint256& txId, uint32_t index) {
    std::string key = buildKey(txId, index);
    rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), key);
    if(!s.ok()) std::cerr << "Remove UTXO failed: " << s.ToString() << std::endl;
}

bool UTXOSet::exists(const uint256& txId, uint32_t index) const {
    std::string key = buildKey(txId, index);
    std::string value;
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &value);
    return s.ok();
}

std::vector<UTXOEntry> UTXOSet::getUTXOsForScript(const std::vector<uint8_t>& script) const {
    std::vector<UTXOEntry> result;
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        // skip checkpoint metadata
        if(it->key().ToString() == "_checkpoint_") continue;

        std::string valStr = it->value().ToString();
        std::vector<uint8_t> valBytes(valStr.begin(), valStr.end());
        UTXOEntry u = UTXOEntry::deserialize(valBytes);
        if(u.scriptPubKey == script) result.push_back(u);
    }
    delete it;
    return result;
}

void UTXOSet::printAllUTXOs() const {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    std::cout << "Current UTXO set:\n";
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        if(it->key().ToString() == "_checkpoint_") continue;

        std::string valStr = it->value().ToString();
        std::vector<uint8_t> valBytes(valStr.begin(), valStr.end());
        UTXOEntry u = UTXOEntry::deserialize(valBytes);

        std::cout << "TxID: " << toHex(u.txId)
                  << ", Index: " << u.index
                  << ", Value: " << u.value
                  << ", ScriptLen: " << u.scriptPubKey.size()
                  << std::endl;
    }
    delete it;
}

// ------------------- Checkpoint -------------------
void UTXOSet::saveCheckpoint(int blockHeight) {
    std::string key = "_checkpoint_";
    std::string value = std::to_string(blockHeight);
    rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
    if(!s.ok()) std::cerr << "Failed to save checkpoint: " << s.ToString() << std::endl;
}

int UTXOSet::getCheckpoint() const {
    std::string key = "_checkpoint_";
    std::string value;
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &value);

    if(s.ok()) return std::stoi(value);
    if(s.IsNotFound()) return -1;
    std::cerr << "Error reading checkpoint: " << s.ToString() << std::endl;
    return -1;
}

void UTXOSet::clearAllUTXOs() {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    int count = 0;
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        if(it->key().ToString() != "_checkpoint_") {
            db->Delete(rocksdb::WriteOptions(), it->key());
            count++;
        }
    }
    delete it;
    std::cout << "Cleared " << count << " UTXOs from database\n";
}

int UTXOSet::getUTXOCount() const {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    int count = 0;
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        if(it->key().ToString() != "_checkpoint_") count++;
    }
    delete it;
    return count;
}
