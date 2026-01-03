// #include <iostream>
// #include <iomanip> // modified
// #include <sstream>
// #include <string>
// #include <openssl/sha.h>
// #include <iostream>
// #include <filesystem>
// #include <string>

// namespace fs = std::filesystem;

// int main() {
//     std::string directory_path = "new_folder"; // Relative path
//     // Or an absolute path: "C:/Users/Example/new_folder" (use forward slashes for portability)

//     try {
//         if (fs::create_directory(directory_path)) {
//             std::cout << "Directory '" << directory_path << "' created successfully." << std::endl;
//         } else {
//             std::cout << "Directory '" << directory_path << "' already exists (or failed for another reason)." << std::endl;
//         }
//     } catch (const fs::filesystem_error& e) {
//         std::cerr << "Error creating directory: " << e.what() << std::endl;
//     }

//     return 0;
// }

// #include <iostream>
// #include "include/dataStructure/vector.h"
// #include "include/dataStructure/hashMap.h"
// using namespace std;

// template <>
// struct Hash<float>
// {
//     size_t operator()(const float &key) const
//     {
//         int hash = (int)key;
//         return hash;
//     }
// };
// int main()
// {
//     Vector<string> vc;

//     vc.push_back("Assalamualikum");
//     vc.push_back("Umar");
//     vc.push_back("Rana");
//     vc.push_back("G");
//     vc.push_back("Kiya");
//     vc.push_back("Hal");
//     vc.push_back("Ha");
//     for (auto vec = vc.begin(); vec != vc.end(); ++vec)
//     {
//         cout << *vec << " ";
//     }

//     cout << endl;
//     HashMap<float, int> map;

//     map.insert(1.2, 10);
//     map.insert(2.3, 20);
//     map.insert(3.3, 30);

//     for (auto it = map.begin(); it != map.end(); ++it)
//     {
//         cout << it->key << " => " << it->value << endl;
//     }
//     return 0;
// }