/*
 * FileUtils.h
 *
 *  Created on: Apr 8, 2019
 *
 */

#ifndef FILEUTILS_H_
#define FILEUTILS_H_
#include <string>
#include <vector>
#include "EventRegistry.h"

namespace license {

std::vector<std::string> filter_existing_files(const std::vector<std::string>& fileList,EventRegistry& registry, const char* extraData);
std::string get_file_contents(const char *filename,size_t max_size);
std::string remove_extension(const std::string& path);

} /* namespace license */

#endif
