#include <iostream>
#include <string>
#include "bimap.h"

int main ()
{
	// maxy::bimap<long, long> failed_map; // key and value types must be different

	std::cout << "Unordered bimap:\n";

	maxy::unordered_bimap<int, std::string> bm;

	bm.set (100, "ururu");
	bm.set (2, "ololo");

	auto ins = bm.insert (3, "tralala");
	std::cout << "insert 3 tralala: [[" << ins.first->first << ", " << ins.first->second << "], " << ins.second << "]\n";
	ins = bm.insert (4, "ololo");
	std::cout << "insert 4 ololo: [[" << ins.first->first << ", " << ins.first->second << "], " << ins.second << "]\n";
	ins = bm.insert (5, "azaza");
	std::cout << "insert 5 azaza: [[" << ins.first->first << ", " << ins.first->second << "], " << ins.second << "]\n";

	std::cout << "bm[3] = " << bm[3]->second << "\n";
	std::cout << "bm[\"ololo\"] = " << bm["ololo"]->first << "\n";

	auto x = bm[123];
	std::cout << "bm[123] = " << (x ? x->second : " nullptr ") << "\n";
	x = bm["azaza"];
	std::cout << "bm[\"azaza\"] = ";
	if (x) std::cout << x->first; else std::cout << " nullptr ";
	std::cout << "\n";
	bm.erase (5);
	std::cout << "bm.erase(5); bm[\"azaza\"] = ";
	x = bm["azaza"];
	if (x) std::cout << x->first; else std::cout << " nullptr ";
	std::cout << "\n";

	int c = 0;
	std::cout << "\niterate keys\n";
	for (auto e : bm)
	{
		std::cout << ++c << ": " << e->first << " => " << e->second << "\n";
	}

	c = 0;
	std::cout << "iterate values\n";
	for (auto b = bm.vbegin (); b != bm.vend (); b++)
	{
		std::cout << ++c << ": " << (*b)->first << " => " << (*b)->second << "\n";
	}

	//auto fail = bm.rbegin (); // this must fail as unordered_map does not have a reverse iterator


	maxy::bimap<int, std::string> obm;

	obm.insert (1, "sdfg");
	obm.insert (10, "zxcv");
	obm.insert (5, "asdf");
	obm.insert (-1, "iuyt");

	std::cout << "\n\nOrdered bimap:\n";

	c = 0;
	std::cout << "iterate keys\n";
	for (auto e : obm)
		std::cout << ++c << ": " << e->first << " => " << e->second << "\n";

	c = 0;
	std::cout << "iterate values\n";
	for (auto b = obm.vbegin (); b != obm.vend (); b++)
		std::cout << ++c << ": " << (*b)->first << " => " << (*b)->second << "\n";

	c = 0;
	std::cout << "reverse iterate keys \n";
	for (auto rk = obm.rbegin (); rk != obm.rend (); rk++)
		std::cout << ++c << ": " << (*rk)->first << " => " << (*rk)->second << "\n";

	c = 0;
	std::cout << "reverse iterate values \n";
	for (auto rv = obm.rvbegin (); rv != obm.rvend (); rv++)
		std::cout << ++c << ": " << (*rv)->first << " => " << (*rv)->second << "\n";

}