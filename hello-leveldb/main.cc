#include <iostream>
#include <string>
#include <cassert>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

int main() {
    // 打印欢迎信息
    std::cout << "LevelDB 基本操作示例" << std::endl;
    std::cout << "=====================" << std::endl;

    // 设置数据库选项
    leveldb::Options options;
    options.create_if_missing = true;  // 如果数据库不存在则创建
    
    // 打开数据库
    leveldb::DB* db = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
    
    if (!status.ok()) {
        std::cerr << "无法打开数据库: " << status.ToString() << std::endl;
        return 1;
    }
    
    std::cout << "数据库成功打开" << std::endl;

    // 写入操作
    std::cout << "\n1. 写入键值对" << std::endl;
    status = db->Put(leveldb::WriteOptions(), "key1", "value1");
    assert(status.ok());
    std::cout << "写入: key1 -> value1" << std::endl;
    
    status = db->Put(leveldb::WriteOptions(), "key2", "value2");
    assert(status.ok());
    std::cout << "写入: key2 -> value2" << std::endl;
    
    status = db->Put(leveldb::WriteOptions(), "key3", "value3");
    assert(status.ok());
    std::cout << "写入: key3 -> value3" << std::endl;

    // 读取操作
    std::cout << "\n2. 读取键值对" << std::endl;
    std::string value;
    status = db->Get(leveldb::ReadOptions(), "key1", &value);
    if (status.ok()) {
        std::cout << "读取 key1: " << value << std::endl;
    } else {
        std::cout << "读取 key1 失败: " << status.ToString() << std::endl;
    }

    // 批量写入操作
    std::cout << "\n3. 批量写入操作" << std::endl;
    leveldb::WriteBatch batch;
    batch.Delete("key1");  // 删除一个键
    batch.Put("key4", "value4");  // 添加一个新键值对
    batch.Put("key5", "value5");  // 添加另一个新键值对
    
    status = db->Write(leveldb::WriteOptions(), &batch);
    assert(status.ok());
    std::cout << "批量操作完成: 删除 key1, 添加 key4 和 key5" << std::endl;

    // 验证删除操作
    status = db->Get(leveldb::ReadOptions(), "key1", &value);
    if (status.IsNotFound()) {
        std::cout << "验证: key1 已被删除" << std::endl;
    } else {
        std::cout << "错误: key1 应该已被删除" << std::endl;
    }

    // 遍历所有键值对
    std::cout << "\n4. 遍历所有键值对" << std::endl;
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::cout << it->key().ToString() << " -> " << it->value().ToString() << std::endl;
    }
    
    if (!it->status().ok()) {
        std::cerr << "遍历出错: " << it->status().ToString() << std::endl;
    }
    
    delete it;  // 不要忘记释放迭代器

    // 关闭数据库
    delete db;
    std::cout << "\n数据库已关闭" << std::endl;
    
    return 0;
}