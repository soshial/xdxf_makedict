#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <algorithm>
#include <cstring>
#include <glib.h>
#include <map>
#include <string>
#include <vector>

template <typename T, typename unref_res_t, void (*unref_res)(unref_res_t *)>
class ResourceWrapper {
public: 
        ResourceWrapper(T *p = NULL) : p_(p) {}
        ~ResourceWrapper() { free_resource(); }
        T *operator->() const { return p_; }
        bool operator!() const { return p_ == NULL; }

        void reset(T *newp) {
                if (p_ != newp) {
                        free_resource();
                        p_ = newp;
                }
        }

        friend inline T *get_impl(const ResourceWrapper& rw) {
                return rw.p_;
        }

        friend inline T **get_addr(ResourceWrapper& rw) {
                return &rw.p_;
        }
private:
        T *p_;

        void free_resource() { if (p_) unref_res(p_); }

// Helper for enabling 'if (sp)'
        struct Tester {
            Tester() {}
        private:
            void operator delete(void*);
        };
public:
// enable 'if (sp)'
        operator const Tester*() const
        {
            if (!*this) return 0;
            static Tester t;
            return &t;
        }
};

namespace glib {
	typedef ResourceWrapper<gchar, void, g_free> CharStr;
	typedef ResourceWrapper<GError, GError, g_error_free> Error;
};

struct less_str {
  bool operator()(const char *lh, const char *rh) const {
    return strcmp(lh, rh)<0;
  }
};
typedef std::map<const char*, const char *, less_str> Str2StrTable;

extern void replace(const Str2StrTable& replace_table,
		    const char *str, std::string& res);

extern bool make_directory(const std::string& dir);

static inline void tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), g_ascii_tolower);
}

static inline bool is_file_exist(const std::string& file)
{
	return g_file_test(file.c_str(), G_FILE_TEST_EXISTS);
}

extern std::vector<std::string> split(const std::string& str, char sep);
extern std::string& strip(std::string& str);
extern const char *b64_encode(guint32 val);
extern guint32 b64_decode(const char *val);
extern void copy_file(const std::string& from, const std::string& to);

#endif//!_UTILS_HPP_
