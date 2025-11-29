#include <IOUring.hpp>

static
void Usage()
{
    printf("Usage:\n");
    exit(1);
}

int main(int argc, char** argv) {
    for (int i=0;i<argc;i++)
    {
        std::string arg{argv[i]};
        if (arg == "--help")
        {
            Usage();
        }
    }


    network::IOUring ring;

    return 0;
}
