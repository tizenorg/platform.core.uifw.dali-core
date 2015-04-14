#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_degree_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_degree_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Positive test case for constructors
int UtcDaliDegreeConstructors01(void)
{
  TestApplication application;

  // Default constructor, does not initialise the value
  Degree degree0( 0.0f );

  // Test assignment operator
  degree0 = Degree(180.0f);
  DALI_TEST_EQUALS( float(degree0), 180.0f, 0.001f, TEST_LOCATION );

  // Constructor from float value
  Degree degree1( 180.0f );
  DALI_TEST_EQUALS( float(degree1), 180.0f, 0.001f, TEST_LOCATION );

  // Constructor from a Radian
  Degree degree2( Radian( Math::PI ) );
  DALI_TEST_EQUALS( float(degree2), 180.0f, 0.001f, TEST_LOCATION );

  END_TEST;
}

// Positive test case for comparison
int UtcDaliDegreeComparison01(void)
{
  TestApplication application;

  // Comparison between radians
  Degree degree0( 90.0f );
  Degree degree1( 90.0f );
  Degree degree2( 180.0f );

  DALI_TEST_CHECK( degree0 == degree1 );
  DALI_TEST_CHECK( degree0 != degree2 );

  // Comparison between radian to degree
  Degree degree3( 180.0f );
  Degree degree4( 90.0f );
  Radian radian0( Math::PI );

  DALI_TEST_CHECK( degree3 == Degree(radian0) );
  DALI_TEST_CHECK( degree4 != Degree(radian0) );

  // Comparison with float
  Degree degree5( 90.0f );

  DALI_TEST_CHECK( degree5 == 90.0f );
  DALI_TEST_CHECK( degree5 != 180.0f );

  END_TEST;
}


// test case for cast operators
int UtcDaliDegreeCastOperators01(void)
{
  TestApplication application;  // Exceptions require TestApplication

  Degree degree0( 180.0f );

  const float& value0( degree0 );
  DALI_TEST_EQUALS( value0, 180.0f, 0.001f, TEST_LOCATION );

  degree0 = Degree( 90.0f );
  DALI_TEST_EQUALS( degree0, Degree(90.0f), 0.001f, TEST_LOCATION );

  float value1( degree0 );
  DALI_TEST_EQUALS( value1, 90.0f, 0.001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliDegreeCastOperatorEquals(void)
{
  TestApplication application;

  Degree a(90.0f);
  Degree b(90.0f);
  Degree c(180.0f);

  DALI_TEST_EQUALS(a == a, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == b, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == c, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDegreeCastOperatorNotEquals(void)
{
  TestApplication application;

  Degree a(90.0f);
  Degree b(90.0f);
  Degree c(180.0f);

  DALI_TEST_EQUALS(a != a, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != c, true, TEST_LOCATION);
  END_TEST;
}
