#include "utility.h"
#include <hash-library/md5.h>
#include "jcontainers_wrapper.h"

namespace slavetats_ng
{
	namespace utility
	{	
		std::string System::md5_hash(const std::string& a_filename)
		{
			std::ifstream file;
			std::istream* input = NULL;
			const size_t  buffer_size = 144 * 7 * 1024;
			char*         buffer = new char[buffer_size];
			MD5           digest;

			file.open(a_filename.c_str(), std::ios::in | std::ios::binary);
			if (!file) {
				logger::info("Could not open '{}'", a_filename);
				return std::string();
			}
			input = &file;
			while (*input) {
				input->read(buffer, buffer_size);
				std::size_t num_read = size_t(input->gcount());
				digest.add(buffer, num_read);
			}
			file.close();
			delete[] buffer;
			return digest.getHash();
		}
	}
}


