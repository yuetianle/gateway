#pragma once

#include <string>
#include "vistek_file_util.h"
using namespace std;
class vistek_mapping_base
{
protected:
	string m_name;
	shared_ptr<void> m_map_handle;
public:
	vistek_mapping_base(const string& name);
	virtual ~vistek_mapping_base()
	{
		
	}
public:
	std::string name() const { return m_name; }
	void* get_map_handle() { return m_map_handle.get(); }
	virtual void close()
	{
		if (m_map_handle != nullptr)
		{
			m_map_handle.reset();
		}
	}

	virtual bool open(int size) = 0;
	
	virtual shared_ptr<void> get_accsessor(int64_t offset, int size) = 0;
};
typedef shared_ptr<vistek_mapping_base> vistek_mapping_ptr;

class vistek_mapping_memory : public vistek_mapping_base
{
protected:
	shared_ptr<void> m_mem_handle;
public:
	vistek_mapping_memory(const string& name);
	virtual ~vistek_mapping_memory();

	virtual bool create(int size);

	virtual bool open(int size)override;
	virtual shared_ptr<void> get_accsessor(int64_t offset, int size)override;
	virtual void close() override;
};
typedef shared_ptr<vistek_mapping_memory> vistek_mapping_memory_ptr;

class vistek_mapping_file : public vistek_mapping_base
{
protected:
	/*shared_ptr<void> m_file_handle;*/
public:
	std::string name() const { return m_name; }
	vistek_mapping_file(const string& filename);
	virtual ~vistek_mapping_file();
	virtual bool open(int size = 0x40000000) override;

	virtual shared_ptr<void> get_accsessor(int64_t offset, int size);
	virtual void close() override;
};
typedef shared_ptr<vistek_mapping_file> vistek_mapping_file_ptr;

