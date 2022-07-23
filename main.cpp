#include <iostream>

#include "StaticRefl.h"
#include "Function.h"
#include "MetaGets.h"

/*
struct Point3D
{
	double x;
	double y;
	double z;
};
*/
DEFINE_STRUCT(
	Point3D,
	(double) x,
	(double) y,
	(double) z
);

std::ostream& operator<<(std::ostream& os, const Point3D& p)
{
	return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

/*
struct Line
{
	Point3D p1;
	Point3D p2;
};
*/
DEFINE_STRUCT(
	Line,
	(Point3D) p1,
	(Point3D) p2
);

std::ostream& operator<<(std::ostream& os, const Line& line)
{
	return os << "start: " << line.p1 << " end: " << line.p2;
}

/*
struct NamedLine
{
	Line line;
	std::string name;
};
*/

DEFINE_STRUCT(
	NamedLine,
	(Line) line,
	(std::string) name
); 

struct Test_t
{
	std::string name;
	int i;

	Test_t()
	{
		std::cout << "Test Constructor: " << this << std::endl;
	}

	Test_t(const std::string& name)
	{
		std::cout << "Test Constructor: " << this << std::endl;
	}

	Test_t(const std::string& name, int i)
		: name(name)
		, i(i)
	{
		std::cout << "Test Constructor: " << this << std::endl;
	}

	Test_t(const Test_t& rhs)
	{

		std::cout << "Test Copy Constructor: " << this << std::endl;
	}

	~Test_t()
	{
		std::cout << "Test Destructor: " << this << std::endl;
	}

	friend std::ostream& operator<<(std::ostream& os, Test_t& t)
	{
		return os << &t;
	}
};

/*
struct FTest
{
	Test_t test;
};
*/
DEFINE_STRUCT(
	FTest,
	(Test_t) test
);

struct FPrintForEach
{
	std::ostream& _os;

	FPrintForEach(std::ostream& os)
		: _os(os)
	{ }

	template <typename TValue>
	void operator()(const char* name, TValue& value)
	{
		_os << name << ": " << value << std::endl;
	}
};

template <typename T>
struct TCopyForEach
{
	T& _out;

	TCopyForEach(T& out)
		: _out(out)
	{ }

	template <size_t TMetaIdx, typename TValue>
	void operator()(const char* name, TValue&& value)
	{
		_out.template meta_set<TMetaIdx>(std::forward<TValue>(value));
	}
};

struct FValueListForEach
{
	std::ostream& _os;

	FValueListForEach(std::ostream& os)
		: _os(os)
	{ }

	template <typename TValue>
	void operator()(TValue&& value)
	{
		_os << value << " ";
	}

	template <size_t TIdx, typename TValue>
	void operator()(TValue&& value)
	{
		_os << "\t" << std::to_string(TIdx) << ": " << value << std::endl;;
	}
};

class CRedis
{
public:
    template <typename T>
    void set(const std::string&, T&&)
    { }


    template <typename T>
    void get(const std::string&, T&&)
    { }
};

/*
 * for static reflected object we can handle the object each meta data [name] and value. 
 * name can be meta_name or vairable_name.field_name.sub_field_name.[...].meta_name 
 */
template <typename T>
struct TRedisSetForEach
{
	CRedis& _handler;

	TRedisSetForEach(CRedis& handler)
		: _handler(handler)
	{ }

	template <typename TValue>
	void operator()(const char* key, TValue&& value)
	{
		_handler.set(key, std::forward<TValue>(value));
	}
};

/*
 * for static reflected object we can handle the object each meta data [name] and value. 
 * name can be meta_name or vairable_name.field_name.sub_field_name.[...].meta_name 
 */
template <typename T>
struct TRedisGetForEach
{
	CRedis& _handler;

	TRedisGetForEach(CRedis& handler)
		: _handler(handler)
	{ }

	template <typename TValue>
	void operator()(const char* key, TValue& value)
	{
		_handler.get(key, value);
	}
};

int main(int argc, char const *argv[])
{
	struct TestHelper
	{
		TestHelper()
		{
			std::cout << "********** Start Static Reflected Test **********\n" << std::endl;
		}

		~TestHelper()
		{
			std::cout << "\n**********  End Static Reflected Test  **********" << std::endl;
		}
	} __helper;

	Point3D p{ 1.1, 0.0 };

	auto& p_y = p.get<1>(); // p.y
	p_y = 2.2;

	p.get<2>() = 3.3;   // p.z
	p.set<0>(0.12345f); // p.x

	// print the p each meta data
	std::cout << "\n-------------- point --------------\n";
	for_each(p, FPrintForEach(std::cout));
	/* OUTPUT:
		x: 0.12345
		y: 2.2
		z: 3.3
	*/

	Line line{ p };
	line.meta_set<3>(3.33f); // line.p2.x
	line.meta_set<4>(4);     // line.p2.y

	// print the line each meta data
	std::cout << "\n-------------- line --------------\n";
	for_each("line", line, FPrintForEach(std::cout));
	/* OUTPUT:
		line.p1.x: 0.12345
		line.p1.y: 2.2
		line.p1.z: 3.3
		line.p2.x: 3.33
		line.p2.y: 4
		line.p2.z: 0	
	*/

	NamedLine nline{ };
	nline.meta_set<0>(0.111);   // nline.p1.x
	nline.meta_set<1>(-2222.2); // nline.p1.y
	nline.meta_set<3>(3.33333); // nline.p2.x
	nline.meta_set<4>(444);     // nline.p2.y
	nline.meta_set<5>(555.555); // nline.p2.z
	nline.meta_set<6>("str");   // nline.name
	nline.meta_set<7>(-777);    // nline.p1.x
	nline.meta_set<9>('9');     // nline.p1.z
	// print the nline each meta data
	std::cout << "\n-------------- nline --------------\n";
	for_each("nline", nline, FPrintForEach(std::cout));
	/* OUTPUT:
		nline.line.p1.x: -777
		nline.line.p1.y: -2222.2
		nline.line.p1.z: 57
		nline.line.p2.x: 3.33333
		nline.line.p2.y: 444
		nline.line.p2.z: 555.555
		nline.name: str	
	*/

	NamedLine out_nline{ };
	// copy nline each meta data to out_nline
	for_each_ex(nline, TCopyForEach<NamedLine>(out_nline));
	// value list of: out_line.p1.x out_line.name out_line.p1.y out_line.p2.x
	auto vs = meta_gets<0, 6, 1, 3>(out_nline); 
	// const reference of vs
	const auto& cr_vs = vs;

	// print the cr_vs
	std::cout << "\nvalue list of out_line field[0, 6, 1, 3]: ";
	for_each(cr_vs, FValueListForEach(std::cout));
	std::cout << std::endl;
	/* OUTPUT:
		value list of out_line field[0, 6, 1, 3]: -777 str -2222.2 3.33333 
	*/

	using type = decltype(vs)::copy_type_list;
	// assign value list of: out_line.p1.x out_line.name out_line.p1.y out_line.p2.x
	vs = type{ 10.0567, "test", 1.1, 6666.666 };

	auto g = gets<2, 1>(vs); // out_line.p1.y out_line.name 
	auto l_n = gets<1>(g);
	l_n = decltype(l_n)::copy_type_list{ "__NAMED_LINE__" };  // out_line.name
	auto l_p1_x = gets<0>(g); // out_line.p1.y
	l_p1_x = decltype(l_p1_x)::copy_type_list{ 123456 };

	// print the out_nline each meta data
	std::cout << "\n-------------- out_nline --------------\n";
	for_each("out_nline", out_nline, FPrintForEach(std::cout));
	/* OUTPUT:
		out_nline.line.p1.x: 10.0567
		out_nline.line.p1.y: 123456
		out_nline.line.p1.z: 57
		out_nline.line.p2.x: 6666.67
		out_nline.line.p2.y: 444
		out_nline.line.p2.z: 555.555
		out_nline.name: __NAMED_LINE__
	*/

	// out_line.p1.x out_line.name out_line.p1.y out_line.p2.x
	std::cout << "\nvalue list of out_line field[0, 6, 1, 3]: \n";
	for_each_ex(cr_vs, FValueListForEach(std::cout));
	/* OUTPUT:
		value list of out_line field[0, 6, 1, 3]: 
			0: 10.0567
			1: __NAMED_LINE__
			2: 123456
			3: 6666.67
	*/

	// print the out_nline.name out_nline.line
	std::cout << "\nvalue list of out_line field[1, 0]: ";
	for_each(gets<1, 0>(out_nline), FValueListForEach(std::cout));
	std::cout << std::endl;
	/* OUTPUT:
		value list of out_line field[1, 0]: __NAMED_LINE__ start: (10.0567, 123456, 57) end: (3.3, 444, 555.555)
	*/

	std::cout << "\nfor_each cost test: \n";
	FTest test;
	// set the meta data of test.test 
	// same as test.test = Test_t{ "123", 1 };
	std::cout << "set or meta_set args cost: \n";
	test.meta_set<0>("123", 1);

	// print the test each meta data
	std::cout << "\n-------------- test --------------\n";
	for_each("test", test, FPrintForEach(std::cout));
	
	return 0; 
}
/* OUTPUT
********** Start Static Reflected Test **********

-------------- point --------------
x: 0.12345
y: 2.2
z: 3.3

-------------- line --------------
line.p1.x: 0.12345
line.p1.y: 2.2
line.p1.z: 3.3
line.p2.x: 3.33
line.p2.y: 4
line.p2.z: 0

-------------- nline --------------
nline.line.p1.x: -777
nline.line.p1.y: -2222.2
nline.line.p1.z: 57
nline.line.p2.x: 3.33333
nline.line.p2.y: 444
nline.line.p2.z: 555.555
nline.name: str

value list of out_line field[0, 6, 1, 3]: -777 str -2222.2 3.33333 

-------------- out_nline --------------
out_nline.line.p1.x: 10.0567
out_nline.line.p1.y: 123456
out_nline.line.p1.z: 57
out_nline.line.p2.x: 6666.67
out_nline.line.p2.y: 444
out_nline.line.p2.z: 555.555
out_nline.name: __NAMED_LINE__

value list of out_line field[0, 6, 1, 3]: 
        0: 10.0567
        1: __NAMED_LINE__
        2: 123456
        3: 6666.67

value list of out_line field[1, 0]: __NAMED_LINE__ start: (10.0567, 123456, 57) end: (6666.67, 444, 555.555) 

for_each cost test: 
Test Constructor: 0x7fffffffd8b0
set or meta_set args cost: 
Test Constructor: 0x7fffffffd770
Test Destructor: 0x7fffffffd770

-------------- test --------------
test.test: 0x7fffffffd8b0
Test Destructor: 0x7fffffffd8b0

**********  End Static Reflected Test  **********
*/

