#ifndef __REPOSITORY_HPP__
#define __REPOSITORY_HPP__

#include <list>
#include <string>

typedef std::vector<std::string> StringList;

template <class T, typename ResType>
class CodecsRepo {
public:
	static ResType& get_instance();
	bool register_codec(T *codec);
	StringList supported_formats() const;
	T *find_codec(const std::string& format);
protected:
	typedef std::list<T *> CodecsList;
	CodecsList codecs_;
};

template <class T, typename R>
R& CodecsRepo<T, R>::get_instance()
{
	static R repo;
	return repo;
}

template <class T, typename R>
bool CodecsRepo<T, R>::register_codec(T *codec)
{
	if (std::find(codecs_.begin(), codecs_.end(), codec) !=
	    codecs_.end())
		return false;
	codecs_.push_back(codec);
	return true;
}

template <class T, typename R>
StringList CodecsRepo<T, R>::supported_formats() const
{
	StringList res;
	typename CodecsList::const_iterator it;

	for (it = codecs_.begin(); it != codecs_.end(); ++it)
		res.push_back((*it)->format());

	return res;
}

template <class T, typename R>
T *CodecsRepo<T, R>::find_codec(const std::string& format)
{
	typename CodecsList::const_iterator it;
	for (it = codecs_.begin(); it != codecs_.end(); ++it)
		if ((*it)->format() == format)
			return *it;

	return NULL;
}



#endif//!__REPOSITORY_HPP__
