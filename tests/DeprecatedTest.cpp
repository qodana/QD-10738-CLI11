// Copyright (c) 2017-2020, University of Cincinnati, developed by Henry Schreiner
// under NSF AWARD 1414736 and by the respective contributors.
// All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include "app_helper.hpp"

using Catch::Matchers::Contains;

TEST_CASE("Deprecated: Empty", "[deprecated]") {
    // No deprecated features at this time.
    CHECK(true);
}

// Classic sets

TEST_CASE_METHOD(TApp, "SetWithDefaults", "[deprecated]") {
    int someint = 2;
    app.add_set("-a", someint, {1, 2, 3, 4}, "", true);

    args = {"-a1", "-a2"};

    CHECK_THROWS_AS(run(), CLI::ArgumentMismatch);
}

TEST_CASE_METHOD(TApp, "SetWithDefaultsConversion", "[deprecated]") {
    int someint = 2;
    app.add_set("-a", someint, {1, 2, 3, 4}, "", true);

    args = {"-a", "hi"};

    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE_METHOD(TApp, "InSet", "[deprecated]") {

    std::string choice;
    app.add_set("-q,--quick", choice, {"one", "two", "three"});

    args = {"--quick", "two"};

    run();
    CHECK(choice == "two");

    args = {"--quick", "four"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE_METHOD(TApp, "InSetWithDefault", "[deprecated]") {

    std::string choice = "one";
    app.add_set("-q,--quick", choice, {"one", "two", "three"}, "", true);

    run();
    CHECK(choice == "one");

    args = {"--quick", "two"};

    run();
    CHECK(choice == "two");

    args = {"--quick", "four"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE_METHOD(TApp, "InIntSet", "[deprecated]") {

    int choice;
    app.add_set("-q,--quick", choice, {1, 2, 3});

    args = {"--quick", "2"};

    run();
    CHECK(choice == 2);

    args = {"--quick", "4"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE_METHOD(TApp, "InIntSetWindows", "[deprecated]") {

    int choice;
    app.add_set("-q,--quick", choice, {1, 2, 3});
    app.allow_windows_style_options();
    args = {"/q", "2"};

    run();
    CHECK(choice == 2);

    args = {"/q", "4"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);

    args = {"/q4"};
    CHECK_THROWS_AS(run(), CLI::ExtrasError);
}

TEST_CASE_METHOD(TApp, "FailSet", "[deprecated]") {

    int choice;
    app.add_set("-q,--quick", choice, {1, 2, 3});

    args = {"--quick", "3", "--quick=2"};
    CHECK_THROWS_AS(run(), CLI::ArgumentMismatch);

    args = {"--quick=hello"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE_METHOD(TApp, "FailMutableSet", "[deprecated]") {

    int choice;
    std::set<int> vals{1, 2, 3};
    app.add_mutable_set("-q,--quick", choice, vals);
    app.add_mutable_set("-s,--slow", choice, vals, "", true);

    args = {"--quick=hello"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);

    args = {"--slow=hello"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

// #113
TEST_CASE_METHOD(TApp, "AddRemoveSetItems", "[deprecated]") {
    std::set<std::string> items{"TYPE1", "TYPE2", "TYPE3", "TYPE4", "TYPE5"};

    std::string type1, type2;
    app.add_mutable_set("--type1", type1, items);
    app.add_mutable_set("--type2", type2, items, "", true);

    args = {"--type1", "TYPE1", "--type2", "TYPE2"};

    run();
    CHECK("TYPE1" == type1);
    CHECK("TYPE2" == type2);

    items.insert("TYPE6");
    items.insert("TYPE7");

    items.erase("TYPE1");
    items.erase("TYPE2");

    args = {"--type1", "TYPE6", "--type2", "TYPE7"};
    run();
    CHECK("TYPE6" == type1);
    CHECK("TYPE7" == type2);

    args = {"--type1", "TYPE1"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);

    args = {"--type2", "TYPE2"};
    CHECK_THROWS_AS(run(), CLI::ValidationError);
}

TEST_CASE("THelp: Defaults", "[deprecated]") {
    CLI::App app{"My prog"};

    int one{1}, two{2};
    app.add_option("--one", one, "Help for one", true);
    app.add_option("--set", two, "Help for set", true)->check(CLI::IsMember({2, 3, 4}));

    std::string help = app.help();

    CHECK_THAT(help, Contains("--one"));
    CHECK_THAT(help, Contains("--set"));
    CHECK_THAT(help, Contains("1"));
    CHECK_THAT(help, Contains("=2"));
    CHECK_THAT(help, Contains("2,3,4"));
}

TEST_CASE("THelp: VectorOpts", "[deprecated]") {
    CLI::App app{"My prog"};
    std::vector<int> x = {1, 2};
    app.add_option("-q,--quick", x, "", true);

    std::string help = app.help();

    CHECK_THAT(help, Contains("INT=[1,2] ..."));
}

TEST_CASE("THelp: SetLower", "[deprecated]") {
    CLI::App app{"My prog"};

    std::string def{"One"};
    app.add_option("--set", def, "Help for set", true)->check(CLI::IsMember({"oNe", "twO", "THREE"}));

    std::string help = app.help();

    CHECK_THAT(help, Contains("--set"));
    CHECK_THAT(help, Contains("=One"));
    CHECK_THAT(help, Contains("oNe"));
    CHECK_THAT(help, Contains("twO"));
    CHECK_THAT(help, Contains("THREE"));
}

TEST_CASE("THelp: ChangingSetDefaulted", "[deprecated]") {
    CLI::App app;

    std::set<int> vals{1, 2, 3};
    int val = 2;
    app.add_option("--val", val, "", true)->check(CLI::IsMember(&vals));

    std::string help = app.help();

    CHECK_THAT(help, Contains("1"));
    CHECK_THAT(help, !Contains("4"));

    vals.insert(4);
    vals.erase(1);

    help = app.help();

    CHECK_THAT(help, !Contains("1"));
    CHECK_THAT(help, Contains("4"));
}

TEST_CASE("THelp: ChangingCaselessSetDefaulted", "[deprecated]") {
    CLI::App app;

    std::set<std::string> vals{"1", "2", "3"};
    std::string val = "2";
    app.add_option("--val", val, "", true)->check(CLI::IsMember(&vals, CLI::ignore_case));

    std::string help = app.help();

    CHECK_THAT(help, Contains("1"));
    CHECK_THAT(help, !Contains("4"));

    vals.insert("4");
    vals.erase("1");

    help = app.help();

    CHECK_THAT(help, !Contains("1"));
    CHECK_THAT(help, Contains("4"));
}

TEST_CASE_METHOD(TApp, "DefaultOpts", "[deprecated]") {

    int i = 3;
    std::string s = "HI";

    app.add_option("-i,i", i, "", false);
    app.add_option("-s,s", s, "", true);

    args = {"-i2", "9"};

    run();

    CHECK(app.count("i") == 1u);
    CHECK(app.count("-s") == 1u);
    CHECK(i == 2);
    CHECK(s == "9");
}

TEST_CASE_METHOD(TApp, "VectorDefaultedFixedString", "[deprecated]") {
    std::vector<std::string> strvec{"one"};
    std::vector<std::string> answer{"mystring", "mystring2", "mystring3"};

    CLI::Option *opt = app.add_option("-s,--string", strvec, "", true)->expected(3);
    CHECK(opt->get_expected() == 3);

    args = {"--string", "mystring", "mystring2", "mystring3"};
    run();
    CHECK(app.count("--string") == 3u);
    CHECK(strvec == answer);
}

TEST_CASE_METHOD(TApp, "DefaultedResult", "[deprecated]") {
    std::string sval = "NA";
    int ival;
    auto opts = app.add_option("--string", sval, "", true);
    auto optv = app.add_option("--val", ival);
    args = {};
    run();
    CHECK("NA" == sval);
    std::string nString;
    opts->results(nString);
    CHECK("NA" == nString);
    int newIval;
    // CHECK_THROWS_AS (optv->results(newIval), CLI::ConversionError);
    optv->default_str("442");
    optv->results(newIval);
    CHECK(442 == newIval);
}

TEST_CASE_METHOD(TApp, "OptionWithDefaults", "[deprecated]") {
    int someint = 2;
    app.add_option("-a", someint, "", true);

    args = {"-a1", "-a2"};

    CHECK_THROWS_AS(run(), CLI::ArgumentMismatch);
}

// #209
TEST_CASE_METHOD(TApp, "CustomUserSepParse", "[deprecated]") {

    std::vector<int> vals = {1, 2, 3};
    args = {"--idx", "1,2,3"};
    auto opt = app.add_option("--idx", vals)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2, 3}) == vals);
    std::vector<int> vals2;
    // check that the results vector gets the results in the same way
    opt->results(vals2);
    CHECK(vals == vals2);

    app.remove_option(opt);

    app.add_option("--idx", vals, "", true)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2, 3}) == vals);
}

// #209
TEST_CASE_METHOD(TApp, "CustomUserSepParse2", "[deprecated]") {

    std::vector<int> vals = {1, 2, 3};
    args = {"--idx", "1,2,"};
    auto opt = app.add_option("--idx", vals)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2}) == vals);

    app.remove_option(opt);

    app.add_option("--idx", vals, "", true)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2}) == vals);
}
//
// #209
TEST_CASE_METHOD(TApp, "CustomUserSepParse4", "[deprecated]") {

    std::vector<int> vals;
    args = {"--idx", "1,    2"};
    auto opt = app.add_option("--idx", vals, "", true)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2}) == vals);

    app.remove_option(opt);

    app.add_option("--idx", vals)->delimiter(',');
    run();
    CHECK(std::vector<int>({1, 2}) == vals);
}

// #218
TEST_CASE_METHOD(TApp, "CustomUserSepParse5", "[deprecated]") {

    std::vector<std::string> bar;
    args = {"this", "is", "a", "test"};
    auto opt = app.add_option("bar", bar, "bar");
    run();
    CHECK(std::vector<std::string>({"this", "is", "a", "test"}) == bar);

    app.remove_option(opt);
    args = {"this", "is", "a", "test"};
    app.add_option("bar", bar, "bar", true);
    run();
    CHECK(std::vector<std::string>({"this", "is", "a", "test"}) == bar);
}
