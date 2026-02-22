#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
        if (argc != 4) {
        return 1;
    }

    bool encode = (argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == '\0');
    bool decode = (argv[1][0] == '-' && argv[1][1] == 'd' && argv[1][2] == '\0');
    if (!encode && !decode) {
        return 1;
    }

    std::ifstream in(argv[2], std::ios::binary);
    if (!in) {
        return 1;
    }

    std::ofstream out(argv[3], std::ios::binary);
    if (!out) {
        return 1;
    }

    if (encode) {
        int c;
        while ((c = in.get()) != std::ifstream::traits_type::eof()) {
            unsigned char byte = static_cast<unsigned char>(c);
            int run = 1;

            int next;
            while (run < 129 && (next = in.get()) != std::ifstream::traits_type::eof()) {
                if (static_cast<unsigned char>(next) == byte) {
                    ++run;
                } else {
                    in.putback(static_cast<char>(next));
                    break;
                }
            }

            if (run >= 2) {
                int rem = run;
                while (rem > 0) {
                    int seg = (rem > 129) ? 129 : rem;
                    out.put(static_cast<char>(seg - 2));
                    out.put(static_cast<char>(byte));
                    rem -= seg;
                }
            } else {
                char buf[128];
                buf[0] = static_cast<char>(byte);
                int len = 1;

                while (len < 128) {
                    int ch1 = in.get();
                    if (ch1 == std::ifstream::traits_type::eof()) {
                        break;
                    }

                    int ch2 = in.get();
                    if (ch2 == std::ifstream::traits_type::eof()) {
                        buf[len] = static_cast<char>(ch1);
                        ++len;
                        break;
                    }

                    if (static_cast<unsigned char>(ch1) == static_cast<unsigned char>(ch2)) {
                        in.putback(static_cast<char>(ch2));
                        in.putback(static_cast<char>(ch1));
                        break;
                    }

                    buf[len] = static_cast<char>(ch1);
                    ++len;
                    in.putback(static_cast<char>(ch2));
                }

                out.put(static_cast<char>(-len));
                out.write(buf, len);
            }
        }
    } else {
        int b;
        while ((b = in.get()) != std::ifstream::traits_type::eof()) {
            signed char len = static_cast<signed char>(b);
            if (len >= 0) {
                int rep = len + 2;
                int c = in.get();
                if (c == std::ifstream::traits_type::eof()) {
                    return 1;
                }
                unsigned char byte = static_cast<unsigned char>(c);
                for (int i = 0; i < rep; ++i) {
                    out.put(static_cast<char>(byte));
                }
            } else {
                int num = -len;
                for (int i = 0; i < num; ++i) {
                    int c = in.get();
                    if (c == std::ifstream::traits_type::eof()) {
                        return 1;
                    }
                    out.put(static_cast<char>(c));
                }
            }
        }
    }

    return 0;
}

