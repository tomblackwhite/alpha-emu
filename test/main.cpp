#include "clock.hh"
#include "cpu.hh"
#include <chrono>
using namespace std::chrono;
#define BOOST_TEST_MODULE My Test
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(first_test) {
  Clock clock;
  auto start = std::chrono::high_resolution_clock().now();
  clock.Start();

  clock.End(10);
  auto end = std::chrono::high_resolution_clock().now();
  auto timeLength = duration_cast<nanoseconds>(end - start).count();
  int nansMin = 550;
  int nansMax = 570;

  BOOST_TEST_MESSAGE("test start first 时间" << timeLength <<" " << nansMin*10 << " " << nansMax*10);
  bool test = (timeLength <= nansMax * 10) && (timeLength >= nansMin * 10);

  int i = 1;
  BOOST_TEST(i);
  BOOST_TEST(test);
}

BOOST_AUTO_TEST_CASE(second_test){
  CPU cpu;
  cpu.m_AC=0x80;
  cpu.m_instructionSet[0x69].m_executor(0x80);

  BOOST_TEST_MESSAGE(cpu.m_SR.to_string());
  BOOST_TEST(1);

}
