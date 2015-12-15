#ifndef vistek_serialize_helper_h__
#define vistek_serialize_helper_h__
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#define VISTEK_STR(text) #text

#define VISTEK_SERIALIZATION_DECLARE(ar, ver)			\
	friend class boost::serialization::access;			\
	template<class Archive>								\
	void serialize(Archive & (ar), unsigned int(ver))

class vistek_xml_serializable
{

	VISTEK_SERIALIZATION_DECLARE(ar, ver)
	{
	}
public:
	template<typename Archive>
	static string serialize(Archive* ptr)
	{
		stringstream ss;
		boost::archive::xml_oarchive xs(ss);
		string tn = typeid(ptr).name();
		tn = REGEX_REPLACE(tn, REGEX("(\\w+) (\\w+)[ ]{0,1}"), "$2", REGEX_CONSTANTS::format_no_copy);
		try
		{
			xs << boost::serialization::make_nvp(tn.c_str(), ptr);
			string content = ss.str();
			return content;
		}
		catch (exception& e)
		{
			cout << "failed to serialize" << e.what() << endl;
		}
		return "";

	}
	template<typename Archive>
	static bool deserialize(Archive* ptr, const std::string& xml)
	{
		try
		{
			std::istringstream iss(xml);
			boost::archive::xml_iarchive xd(iss);
			string tn = typeid(ptr).name();
			tn = REGEX_REPLACE(tn, REGEX("(\\w+) (\\w+)[ ]{0,1}"), "$2", REGEX_CONSTANTS::format_no_copy);
			xd >> boost::serialization::make_nvp(tn.c_str(), *ptr);
			return true;
		}
		catch (std::exception& e)
		{
			cout << "failed to deserailize " << BOOST_PP_STRINGIZE(Archive) << " failed. error: " << string(e.what()) << endl;;
		}
		return false;
	}

};
class vistek_xml_serializable_ex : public vistek_xml_serializable
{
	VISTEK_SERIALIZATION_DECLARE(ar, ver)
	{
	}
protected:
	static void _add_header(std::string& content);
	static void _remove_header(std::string& content);
	static void _add_attributes(std::string& content, const std::string& nodeName, int classID);
	static void _remove_attributes(std::string& content);
	static void _replace_attributes(std::string& content, const string& nodeName, const string& attr);
public:
	static void transform_array_to_msg(std::string& content, const std::string& nodeName);
	static void transform_array_to_std(std::string& content);
protected:
	int m_class_id;
	int m_tracking_level;
	int m_version;
public:
	vistek_xml_serializable_ex(int classID, int trackingLevel = 0, int version = 0)
		: m_class_id(classID), m_tracking_level(trackingLevel), m_version(version)
	{

	}
	virtual std::string to_string(const std::string& nodeName) = 0;
	virtual bool apply(const std::string& xml, const std::string& nodeName) = 0;

	
	int class_id() const { return m_class_id; }
};
class serialization_sample : public vistek_xml_serializable_ex
{
	VISTEK_SERIALIZATION_DECLARE(ar, ver)
	{
		ar & BOOST_SERIALIZATION_NVP(TestID);
		ar & BOOST_SERIALIZATION_NVP(TestPort);
	}
private:
	int TestID = 0;
	int TestPort = 0;
public:
	serialization_sample()
		: vistek_xml_serializable_ex(0)
	{

	}
	serialization_sample(int classID);
	virtual std::string to_string(const std::string& alias = "Sample_Test")override;
	virtual bool apply(const std::string& content, const std::string& nodeName = "Sample_Test") override;


};
#endif // vistek_serialize_helper_h__