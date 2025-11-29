#include <IOUring.hpp>

static
void Usage()
{
    printf("Usage: --ping <ip>\n");
}

int main(int argc, char** argv) {
    Logger logger(true, true, LogOutput::CONSOLE);

    std::optional<network::IPAddress>  addr_opt;
    for (int i=0;i<argc;i++)
    {
        std::string arg{argv[i]};
        if (arg == "--ping")
        {
            auto in_addr = network::IPAddress::string_to_ipv4_address(argv[i+1], logger);
            network::IPAddress addr( in_addr, 80);
            addr_opt = addr;
        } else if (arg == "--help")
        {
            Usage();
            return 1;
        }
    }

    if (! addr_opt)
    {
        Usage();
        return 1;
    }

    LOG_INFO(logger, "going to ping %s\n", addr_opt.value().to_human_readable_ip_string().c_str());

    network::IOUring io(logger);
    io.submit_connect();

    return 0;
}
