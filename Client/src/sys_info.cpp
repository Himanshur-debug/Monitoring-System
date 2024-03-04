#include<sys_info.h>

// Get the system hostname
std::string getHostname() {
    char hostname[1024];
    gethostname(hostname, 1024);
    return std::string(hostname);
}

// Get the system IP address
std::string getIPAddress() {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    std::string ip_address;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            ip_address = std::string(ip);
            break;
        }
    }

    freeifaddrs(ifaddr);
    return ip_address;
}

// Get CPU usage
double getCPUUsage() {
    // Read /proc/stat and calculate CPU usage
    std::ifstream file("/proc/stat");
    std::string line;
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    getline(file, line);
    std::istringstream iss(line);
    iss >> line >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    long idle_time = idle + iowait;
    long non_idle_time = user + nice + system + irq + softirq + steal;
    long total_time = idle_time + non_idle_time;

    return (1.0 - static_cast<double>(idle_time) / total_time) * 100.0;
}

// Get RAM usage
long getRAMUsage() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    long totalMem = 0, freeMem = 0;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        iss >> key >> value;
        if (key == "MemTotal:") totalMem = value;
        if (key == "MemFree:") freeMem = value;
    }
    return (totalMem - freeMem);
}

// Get network stats
std::string getNetworkStats() {
    std::string interfaces = "";
    std::ifstream file("/proc/net/dev");
    std::string line;
    std::getline(file, line); // Skip the first two lines
    std::getline(file, line);

    // std::cout << "Network Statistics:" << std::endl;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string interface;
        iss >> interface;

        // std::cout << "Interface: " << interface << std::endl;
        // std::cout << "   ";
        interfaces += interface;

        for (int i = 0; i < 8; ++i) {
            std::string data;
            iss >> data;
            interfaces += " " + data;
            // std::cout << data << " ";
        }
        interfaces += "\n";
        // std::cout << std::endl;
    }
    return interfaces;
}
