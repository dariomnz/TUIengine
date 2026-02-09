#pragma once

#include <iostream>

namespace TUIE {

struct FixedBuffer : std::streambuf {
    FILE *m_file;

    FixedBuffer(char *buf, size_t size, FILE *file) : m_file(file) { setp(buf, buf + size); }

    ~FixedBuffer() override { sync(); }

    int_type overflow(int_type c) override {
        if (m_file) {
            flush_to_file();
            if (!traits_type::eq_int_type(c, traits_type::eof())) {
                sputc(traits_type::to_char_type(c));
            }
            return c;
        }
        return traits_type::eof();
    }

    int sync() override {
        if (m_file) {
            flush_to_file();
        }
        return 0;
    }

    void flush_to_file() {
        if (!m_file) return;
        std::ptrdiff_t n = pptr() - pbase();
        if (n > 0) {
            fwrite(pbase(), 1, n, m_file);
            fflush(m_file);
            setp(pbase(), epptr());  // Reset pointers
        }
    }
};

template <size_t Capacity>
struct FixedOStreamStorage {
    char m_buffer[Capacity];
    FixedBuffer m_storage;

    FixedOStreamStorage(FILE *file) : m_storage(m_buffer, Capacity, file) {}
};

template <size_t Capacity>
struct FixedOStream : private FixedOStreamStorage<Capacity>, public std::ostream {
    FixedOStream() : FixedOStreamStorage<Capacity>(nullptr), std::ostream(&this->m_storage) {}
    FixedOStream(FILE *file) : FixedOStreamStorage<Capacity>(file), std::ostream(&this->m_storage) {}

    void clear_buffer() {
        this->m_storage.clear();
        this->clear();
    }

    std::string_view sv() const { return std::string_view(this->m_storage.pbase(), this->m_storage.size()); }
};

template <size_t Capacity>
struct FixedCoutStream : public FixedOStream<Capacity> {
    FixedCoutStream() : FixedOStream<Capacity>(stdout) {}
};

template <size_t Capacity>
struct FixedCerrStream : public FixedOStream<Capacity> {
    FixedCerrStream() : FixedOStream<Capacity>(stderr) {}
};

inline FixedCoutStream<4096> fixedCout;
inline FixedCerrStream<4096> fixedCerr;
}  // namespace TUIE
