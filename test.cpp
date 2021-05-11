# include <iostream>
# include <cstring>


bool doing(){}


void help() {
        std::cout << "Usage: bak [OPTION1] ... [OPTION2] ... [start|restart]" << std::endl;
        std::cout << "       bak [start|stop|restart]\n" << std::endl;
        std::cout << "Backup the whole system automatically.\n" << std::endl;
        std::cout << "With no option nor other arguments, display this help.\n" << std::endl;
        std::cout << "  -b [days]   --backup-interval       to change backup interval." << std::endl;
        std::cout << "  -d [days]   --delete-interval       to change delete interval." << std::endl;
        std::cout << "  -n          --ignore-interval       to ignore backup interval." << std::endl;
        std::cout << "\n" << std::endl;
        std::cout << "This program is developed by czq01. Copyright [2020] czq01." << std::endl;
        std::cout << "School of Economics and management, Beijing Jiaotong University" << std::endl;
        std::cout << "Contact us: ZiquanCheng@outlook.com" << std::endl;
//      std::cout << "" << std::endl;
}

bool stop_task() {}
bool start_task() {}

int main(int argc, char * argv[]) {
    // argv[0] [options] [arguments]
    if (argc == 1) {help(); return -1;}

    const char * status = argv[argc-1];
    if (argc == 2) {
        if (*argv[1] == *"--help") {help(); return 0;}
        else if (*argv[1] == *"stop") {stop_task(); return 0;}
        else if (*argv[1] == *"start") {start_task();return 0;}
        else if (*argv[1] == *"restart") {}
        else {
            std::cout << "bak:  invalid command." << std::endl;
            std::cout << "Try 'bak --help' for more information." << std::endl;
            return -1;
        }
    }
    else if (argc > 2) {
        if(*status == *"stop"){}
    }

    std::cout <<argc<<std::endl;
    return 0;
}