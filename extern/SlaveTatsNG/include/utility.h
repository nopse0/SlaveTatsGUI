#pragma once

namespace slavetats_ng
{
	namespace utility
	{

		class System : public clib_util::singleton::ISingleton<System>
		{
		public:
			inline System() {}
			static std::string md5_hash(const std::string& a_filename);
		};
	}
}




