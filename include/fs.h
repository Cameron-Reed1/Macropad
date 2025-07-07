#include <stddef.h>
#include <iterator>
#include <stdint.h>
#include <lfs.h>


namespace fs {

extern const uint32_t FLASH_OFFSET;
extern const uint8_t* FLASH_START;
extern const uint32_t SIZE;

void init();


class File
{
public:
    File(const char* const path, int flags);
    ~File();

    lfs_size_t read(void* buffer, lfs_size_t size);
    int close();

public:
    int open_err;

private:
    bool closed;
    lfs_file_t file;

private:
    File() = delete;
};


class Dir
{
public:
    Dir(const char* const path);

    int close();

public:
    int open_err;

private:
    lfs_dir_t dir;

private:
    Dir() = delete;

public:
    class iterator
    {
    public:
        using value_type = const lfs_info*;
        using difference_type = ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator();
        iterator(Dir* dir);

        value_type operator*() const;
        iterator& operator++();
        friend bool operator==(const iterator& lhs, const iterator& rhs);
        friend bool operator!=(const iterator& lhs, const iterator& rhs);

    private:
        Dir* m_dir;
        bool m_end;
        lfs_info m_current;
    };

    iterator begin();
    iterator end();
};

} // namespace fs
