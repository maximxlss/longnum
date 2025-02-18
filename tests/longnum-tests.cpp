#include"../src/longnum.hpp"
#include"../tests/utils.hpp"


void test_longnum_conversion() {
    assert_eq("0"_longnum, "0"_longdecimal);
    assert_eq("1"_longnum, "1"_longdecimal);
    assert_eq("1010"_longnum, "10"_longdecimal);
    assert_eq("1010.1"_longnum, "10.5"_longdecimal);
    assert_eq("1010.1"_longnum, "10.5"_longdecimal);
    assert_eq("1011011001111100.1100100111111100100100001010001100111"_longnum, "46716.78901008592"_longdecimal);
    assert_eq("0.00000000000000000000000000001000001101100100010000100100001000001100000011010"_longnum, "0.00000000191199994903277"_longdecimal);
    assert_eq("11000010100110011111.00000111100101001110101010001110101110101101"_longnum, 797087.0296160315_longnum);
    assert_eq("100000000000"_longdecimal, LongNum(1e11));
    assert_eq("-110110001011011.1100101000100001110101010011000010010101001111001"_longnum, -27739.78957874714_longnum);
    assert_eq("0.00000000000000000000000000000000000000000001100000101111011101010001110100000101011101101100101010111010110"_longnum, .00000000000008592373304639695_longnum);
    assert_eq(0_longnum, LongNum(0));
    assert_eq("-0"_longnum, LongNum(0));
    assert_eq("-0"_longdecimal, LongNum(0));
    assert_eq("-"_longnum, LongNum(0));
    assert_eq("-"_longdecimal, LongNum(0));
    assert_eq("- \t "_longdecimal, LongNum(0));
    assert_eq(" -00\n "_longdecimal, LongNum(0));
    assert_eq(LongNum::from_string("4416857.b7f578", 16), LongNum(0x4416857.b7f578p0l));
    assert_eq(LongNum(0x4416857.b7f578p0l).to_string(16), std::string("4416857.b7f578"));
    assert_eq("0.000000000000000000000000000000000000000000011000001011110111010100011101000001010111011011001011"_longnum.to_binary_string(), std::string("0.000000000000000000000000000000000000000000011000001011110111010100011101000001010111011011001011"));
    assert_eq("1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"_longnum.to_binary_string(), std::string("1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"));
    assert_eq("10000"_longnum.to_binary_string(), std::string("10000.0000000000000000000000000000000000000000000000000000000000000000"));
    assert_eq("-0.000000000000000000000000000000000000000000011000001011110111010100011101000001010111011011001011"_longnum.to_binary_string(), std::string("-0.000000000000000000000000000000000000000000011000001011110111010100011101000001010111011011001011"));
    assert_eq("-1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"_longnum.to_binary_string(), std::string("-1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"));
    assert_eq("-10000"_longnum.to_binary_string(), std::string("-10000.0000000000000000000000000000000000000000000000000000000000000000"));
    assert_eq("-1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"_longnum.to_string(2), std::string("-1011011001111100.1100100111111100100100001010001100111101100101001011000010000101"));
    assert_eq(""_longnum, LongNum(0));
    assert_eq("     \t\n  "_longnum, LongNum(0));
    assert_eq("   11001010.10101011  \t\n"_longnum, "11001010.10101011"_longnum);
    assert_eq(LongNum(0).with_precision(0).to_binary_string(), std::string("0"));
    assert_eq(LongNum(0).with_precision(0).to_string(), std::string("0"));
    assert_eq(LongNum(-123).with_precision(0).to_string(), std::string("-123"));
}

void test_longnum_comparison() {
    LongNum x = 380599;
    LongNum y = 850990;
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = 5311.04488;
    y = 275504.40981;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = 0.000385603;
    y = 0.0054135778;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = -45683;
    y = -27758;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = -6542.9342;
    y = -3876.7835;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = -0.00601941;
    y = -0.004695;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = -455.455;
    y = 455.455;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = 552.878;
    y = 552.8781;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = "16816753222879769273857.86176575434261747288223913"_longdecimal;
    y = "17765712314729373366031.295501490221896812082702642454271804539272858"_longdecimal;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = "824198124379180146193.967549727403679899767414927645455431806492246561"_longdecimal;
    y = "824198124379181146193.967549727403679899767414927645455431806492246561"_longdecimal;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = "46883954833035579871856.639243731358012103167224641949268220385329638918194490"_longdecimal;
    y = "46883954833035579871856.639243731358012103167224641949268220385329738918194490"_longdecimal;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));

    x = 0.03775;
    y = 0.03775;
    assert(x == y);
    assert(!(x != y));
    assert(x >= y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x < y));

    x = "6674910537442664749437561000.4429319139011708392772142396627472079314429044840"_longdecimal;
    y = "6674910537442664749437561000.4429319139011708392772142396627472079314429044840"_longdecimal;
    assert(x == y);
    assert(!(x != y));
    assert(x >= y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x < y));

    x = "-6495356892658545804475722787353656.4229216553796079239068316192003429083479778"_longdecimal;
    y = "6495356892658545804475722787353656.4229216553796079239068316192003429083479778"_longdecimal;
    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(!(x > y));
    assert(!(x >= y));
    assert(y > x);
    assert(y >= x);
    assert(!(y < x));
    assert(!(y <= x));
}

void test_longnum_addition_subtraction() {
    LongNum x = 0;
    assert_eq(-x, x);
    assert_eq(-x, LongNum(0));
    assert_eq(x + x, LongNum(0));
    assert_eq(x - x, LongNum(0));
    assert_eq(x + x + x, LongNum(0));
    assert_eq(x + 0 + 0, x);
    assert_eq(0 + 0 + x, x);
    assert_eq(x, LongNum(0));

    x = 1;
    assert_eq(-x, LongNum(-1));
    assert_eq(x - 1, LongNum(0));
    assert_eq(x + x, LongNum(2));
    assert_eq(x - x, LongNum(0));
    assert_eq(x + x + x, LongNum(3));
    assert_eq(x + 0 + 0, x);
    assert_eq(0 + 0 + x, x);
    assert_eq(x, LongNum(1));

    x = -1;
    assert_eq(-x, LongNum(1));
    assert_eq(x + 1, LongNum(0));
    assert_eq(x + x, LongNum(-2));
    assert_eq(x - x, LongNum(0));
    assert_eq(x - x + x, x);
    assert_eq(x + x + x, LongNum(-3));
    assert_eq(x + 0 + 0, x);
    assert_eq(0 + 0 + x, x);
    assert_eq(x, LongNum(-1));

    x = "68145812196212373913311824842040"_longdecimal;
    LongNum y = "6070282394709034116814679016"_longdecimal;
    assert_eq(x + y, "68151882478607082947428639521056"_longdecimal);
    assert_eq(x + y, x - (-y));
    assert_eq(x - y, "68139741913817664879195010163024"_longdecimal);
    assert_eq(x + 0 + 0, x);
    assert_eq(0 + 0 + x, x);
    assert_eq(x - y, x + (-y));
    assert_eq(x - y, (-y) + x);
    assert_eq(x + x, x << 1);
    assert_eq(x + x, x - (-x));
    assert_eq(x - y + y, x);
    assert_eq(y - x, "-68139741913817664879195010163024"_longdecimal);
    assert_eq(y - x, -(x - y));
    assert_eq(y - x, -x + y);
    assert_eq(y - x, y + (-x));
    assert_eq(x, "68145812196212373913311824842040"_longdecimal);
    assert_eq(y, "6070282394709034116814679016"_longdecimal);

    x = "1010010000100000111011100001001010101101000.010010100010100110011100110001110110000011001011100101101100111001"_longnum;
    y = "11100110100101010110111110001100111011111.0000000010011100011111110000011001011110100111010100001001"_longnum;
    assert_eq(x + y, "1101110111000110010010011111010111101000111.010010101100011000011011110011011011111101101000110110010000111001"_longnum);
    assert_eq(x + y, x - (-y));
    assert_eq(x - y, "110101001111011100100100010111101110001001.010010011000110100011101110000010000001000101110010101001000111001"_longnum);
    assert_eq(x + 0 + 0, x);
    assert_eq(0 + 0 + x, x);
    assert_eq(x - y, x + (-y));
    assert_eq(x - y, (-y) + x);
    assert_eq(x + x, x << 1);
    assert_eq(x + x, x - (-x));
    assert_eq(x - y + y, x);
    assert_eq(y - x, -(x - y));
    assert_eq(y - x, -x + y);
    assert_eq(y - x, y + (-x));
    assert_eq(x, "1010010000100000111011100001001010101101000.010010100010100110011100110001110110000011001011100101101100111001"_longnum);
    assert_eq(y, "11100110100101010110111110001100111011111.0000000010011100011111110000011001011110100111010100001001"_longnum);

    assert_eq((LongNum(1) << 31) + (LongNum(1) << 31), LongNum(1) << 32);
}

void test_longnum_shifts() {
    LongNum x = 1;
    assert_eq(x << 1, 2_longnum);
    assert_eq(x << 10, 1024_longnum);
    assert_eq(x << 100, "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_longnum);
    assert_eq(x >> 1, 0.5_longnum);
    assert_eq(x >> 10, 0.0009765625_longnum);
    assert_eq(x >> 1024, LongNum(0));
    assert_eq((x << 128) >> 128, x);
    assert_eq(x << 123, x >> (-123));
    assert_eq(x >> 12, x << (-12));
    assert_eq(x, x >> 0);
    assert_eq(x, x << 0);
    assert_eq(x, LongNum(1));

    x = -1;
    assert_eq(x << 1, -2_longnum);
    assert_eq(x << 10, -1024_longnum);
    assert_eq(x << 100, "-10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_longnum);
    assert_eq(x >> 1, -0.5_longnum);
    assert_eq(x >> 10, -0.0009765625_longnum);
    assert_eq(x >> 1024, LongNum(0));
    assert_eq((x << 128) >> 128, x);
    assert_eq(x << 123, x >> (-123));
    assert_eq(x >> 12, x << (-12));
    assert_eq(x, x >> 0);
    assert_eq(x, x << 0);
    assert_eq(x, LongNum(-1));

    x = "100110101011101011111001000001010010011011000010110001101100001.0111110000101000011010010010010010000011001010101010110001100110"_longnum;
    assert_eq(x << 3, "100110101011101011111001000001010010011011000010110001101100001011.1110000101000011010010010010010000011001010101010110001100110"_longnum);
    assert_eq(x << 70, "1001101010111010111110010000010100100110110000101100011011000010111110000101000011010010010010010000011001010101010110001100110000000"_longnum);
    assert_eq(x >> 7, "10011010101110101111100100000101001001101100001011000110.1100001011111000010100001101001001001001000001100101010101011000"_longnum);
    assert_eq(x >> 30, "100110101011101011111001000001010.0100110110000101100011011000010111110000101000011010010010010010"_longnum);
    assert_eq(-x << 70, "-1001101010111010111110010000010100100110110000101100011011000010111110000101000011010010010010010000011001010101010110001100110000000"_longnum);
    assert_eq(-x >> 7, "-10011010101110101111100100000101001001101100001011000110.1100001011111000010100001101001001001001000001100101010101011000"_longnum);
    assert_eq(-x >> 30, "-100110101011101011111001000001010.0100110110000101100011011000010111110000101000011010010010010010"_longnum);
    assert_eq(x << 123, x >> (-123));
    assert_eq(x >> 50, x << (-50));
    assert_eq(x, x >> 0);
    assert_eq(x, x << 0);
    assert_eq(x, "100110101011101011111001000001010010011011000010110001101100001.0111110000101000011010010010010010000011001010101010110001100110"_longnum);

    x = 0b10100101001010010101010010011_longnum;
    assert_eq(x << 2, 0b1010010100101001010101001001100_longnum);
    assert_eq(x << 34, "101001010010100101010100100110000000000000000000000000000000000"_longnum);
    assert_eq(x, x >> 0);
    assert_eq(x, x << 0);
    assert_eq(x, 0b10100101001010010101010010011_longnum);

    x = 673586480112;
    assert_eq(x << 300, "1372119893160657814808001107988915751593382048158456755060980466137483702613101510546362584033616986112"_longdecimal);
    assert_eq(x, x >> 0);
    assert_eq(x, x << 0);
    assert_eq(x, LongNum(673586480112));
}

void test_longnum_multiplication() {
    assert_eq(LongNum(1) * 1, LongNum(1));
    assert_eq(LongNum(-1) * 1, LongNum(-1));
    assert_eq(LongNum(1) * -1, LongNum(-1));
    assert_eq(LongNum(-1) * -1, LongNum(1));
    assert_eq(LongNum(1) * 0, LongNum(0));
    assert_eq(LongNum(0) * 1, LongNum(0));
    assert_eq(LongNum(0) * 0, LongNum(0));
    assert_eq(LongNum(2) * 1, LongNum(1) + 1);

    LongNum x = "3483096694536044378308"_longdecimal;
    assert_eq(x * 2, x + x);
    assert_eq(x * 2, "6966193389072088756616"_longdecimal);
    assert_eq(x * -1, "-3483096694536044378308"_longdecimal);
    assert_eq(x * -2, "-6966193389072088756616"_longdecimal);
    assert_eq(x << 3, x * 8);
    assert_eq(x * 0, LongNum(0));
    assert_eq(LongNum(0) * x, LongNum(0));
    assert_eq(x, "3483096694536044378308"_longdecimal);
    x *= 2;
    assert_eq(x, "6966193389072088756616"_longdecimal);

    x = "3483096694536044378308"_longdecimal;
    LongNum y = "17508438146505479"_longdecimal;
    assert_eq(x * y, "60983583034582021399174027313270749532"_longdecimal);

    x = "10110110010010011010100110000001.01100000000111001110101100101"_longnum;
    assert_eq(x * y, "10110001001010101111110101011101011011111111001111001101100111010000100010011000010101.10101101000110011010011000011"_longnum);

    y = "111011111100010101011100001000101101000.100111000101010111100000111"_longnum;
    assert_eq(x * y, "10101010101110110100110110001010011111001111011101110101010100010101010.00101111110110010011111001110011110110110101100011000011"_longnum);
}

void test_longnum_division() {
    assert_eq(LongNum(0) / 1, LongNum(0));
    assert_eq(LongNum(1) / 1, LongNum(1));
    assert_eq(LongNum(-1) / 1, LongNum(-1));
    assert_eq(LongNum(1) / -1, LongNum(-1));
    assert_eq(LongNum(-1) / -1, LongNum(1));
    assert_eq(LongNum(1) / 2, LongNum(0.5));
    assert_eq(LongNum(1) / (LongNum(1) << 50), LongNum(1) >> 50);
    assert_eq(LongNum(1) / 2, LongNum(0.5));

    bool thrown = false;
    try {
        LongNum(1) / 0;
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        LongNum(0) / 0;
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    assert(thrown);

    assert_eq((LongNum(22) / 7).to_string().substr(0, 4), std::string("3.14"));
    assert_eq("5574748814014767969.4849916185514"_longdecimal / "25521421424.52151324364"_longdecimal, "1101000001010000101000110111.10110011001100011011111100100011100111001111"_longnum);
    assert_eq("14767969.4849916153285514"_longdecimal / ".000513243642421412"_longdecimal, "11010110011000011010101010011110011.101111000011000000101000111000101100000100011"_longnum);
}

void test_longnum_utils() {
    LongNum x = "1001010100101010101010100101010101010001010101110101000101010101.1001010101010101010101010010010101"_longnum;
    assert(x.get_bit(0));
    assert(!x.get_bit(1));
    assert(!x.get_bit(52));
    assert(x.get_bit(63));
    assert(x.get_bit(-1));
    assert(!x.get_bit(-2));
    assert(x.get_bit(-20));
    assert(x.get_bit(-34));
    x.unset_bit(0);
    x.set_bit(1);
    x.set_bit(52);
    x.unset_bit(63);
    x.unset_bit(-1);
    x.set_bit(-2);
    x.unset_bit(-20);
    x.unset_bit(-34);
    assert(!x.get_bit(0));
    assert(x.get_bit(1));
    assert(x.get_bit(52));
    assert(!x.get_bit(63));
    assert(!x.get_bit(-1));
    assert(x.get_bit(-2));
    assert(!x.get_bit(-20));
    assert(!x.get_bit(-34));

    x = "1001010100101010101010100101010101010001010101110101000101010101.1001010101010101010101010010010101"_longnum;
    assert_eq(x.bit_length(), 64);
    LongNum y = x.truncate();
    assert_eq(y, "1001010100101010101010100101010101010001010101110101000101010101"_longnum);
    assert_eq(y.bit_length(), 64);
    y = x.round();
    assert_eq(y, "1001010100101010101010100101010101010001010101110101000101010110"_longnum);
    assert_eq((x - 0.5).round(), "1001010100101010101010100101010101010001010101110101000101010101"_longnum);
    y = ".000001001001010101"_longnum;
    assert_eq(y.bit_length(), -5);
    y >>= 1;
    assert_eq(y.bit_length(), -6);
    y = x.frac();
    assert_eq(y, ".1001010101010101010101010010010101"_longnum);

    assert_eq(LongNum(123).pow(0), LongNum(1));
    assert_eq(LongNum(123).pow(1), LongNum(123));
    assert_eq(LongNum(123).pow(2), LongNum(15129));
    assert_eq(LongNum(123).pow(3), LongNum(1860867));
    assert_eq(LongNum(123).pow(10), "792594609605189126649"_longdecimal);

    assert_eq(LongNum(123).to_int(), 123);
    assert_eq(LongNum(0).to_int(), 0);
    assert_eq(LongNum(-123).to_int(), -123);
    assert_eq("10001011010101010010101010101001010010101011010100100010101001011.1010100010101010011"_longnum.to_int(), 0b10101011010100100010101001011);
    assert_eq("10100101000010010101000010101001010011011010011.01100101"_longnum.with_precision(8).to_int(), 0b101000010101001010011011010011);

    x.set_precision(123);
    assert_eq(x.precision(), 123u);
    assert_eq(x.with_precision(0), x.truncate());
    assert_eq(x.with_precision(321).precision(), 321u); 
}
