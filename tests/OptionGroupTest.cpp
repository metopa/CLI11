#include "app_helper.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::HasSubstr;
using ::testing::Not;

using vs_t = std::vector<std::string>;

TEST_F(TApp, BasicOptionGroup) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);

    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);
    EXPECT_EQ(app.count_all(), 1u);
}

TEST_F(TApp, BasicOptionGroupExact) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(1);
    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);

    args = {"--test1", "5", "--test2", "4"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[Exactly 1");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupExactTooMany) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(10);
    args = {"--test1", "5"};
    EXPECT_THROW(run(), CLI::InvalidError);
}

TEST_F(TApp, BasicOptionGroupMinMax) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(1, 1);
    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);

    args = {"--test1", "5", "--test2", "4"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[Exactly 1");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMinMaxDifferent) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(1, 2);
    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);

    args = {"--test1", "5", "--test2", "4"};
    EXPECT_NO_THROW(run());
    EXPECT_EQ(app.count_all(), 2u);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[Between 1 and 2");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMinMaxDifferentReversed) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(2, 1);
    EXPECT_EQ(ogroup->get_require_option_min(), 2u);
    EXPECT_EQ(ogroup->get_require_option_max(), 1u);
    args = {"--test1", "5"};
    EXPECT_THROW(run(), CLI::InvalidError);
    ogroup->require_option(1, 2);
    EXPECT_NO_THROW(run());
    EXPECT_EQ(res, 5);
    EXPECT_EQ(ogroup->get_require_option_min(), 1u);
    EXPECT_EQ(ogroup->get_require_option_max(), 2u);
    args = {"--test1", "5", "--test2", "4"};
    EXPECT_NO_THROW(run());

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[Between 1 and 2");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMax) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(-2);
    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);

    args = {"--option", "9"};
    EXPECT_NO_THROW(run());

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[At most 2");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMax1) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(-1);
    args = {"--test1", "5"};
    run();
    EXPECT_EQ(res, 5);

    args = {"--option", "9"};
    EXPECT_NO_THROW(run());

    args = {"--test1", "5", "--test2", "4"};
    EXPECT_THROW(run(), CLI::RequiredError);

    std::string help = ogroup->help();
    auto exactloc = help.find("[At most 1");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMin) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option();

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_NO_THROW(run());

    std::string help = ogroup->help();
    auto exactloc = help.find("[At least 1");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupExact2) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(2);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test3=5"};
    EXPECT_NO_THROW(run());

    std::string help = ogroup->help();
    auto exactloc = help.find("[Exactly 2");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMin2) {
    auto ogroup = app.add_option_group("clusters");
    int res;
    ogroup->add_option("--test1", res);
    ogroup->add_option("--test2", res);
    ogroup->add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);
    ogroup->require_option(2, 0);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_NO_THROW(run());

    std::string help = ogroup->help();
    auto exactloc = help.find("[At least 2");
    EXPECT_NE(exactloc, std::string::npos);
}

TEST_F(TApp, BasicOptionGroupMinMoved) {

    int res;
    auto opt1 = app.add_option("--test1", res);
    auto opt2 = app.add_option("--test2", res);
    auto opt3 = app.add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);

    auto ogroup = app.add_option_group("clusters");
    ogroup->require_option();
    ogroup->add_option(opt1);
    ogroup->add_option(opt2);
    ogroup->add_option(opt3);

    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_NO_THROW(run());

    std::string help = app.help();
    auto exactloc = help.find("[At least 1");
    auto oloc = help.find("--test1");
    EXPECT_NE(exactloc, std::string::npos);
    EXPECT_NE(oloc, std::string::npos);
    EXPECT_LT(exactloc, oloc);
}

TEST_F(TApp, BasicOptionGroupMinMovedAsGroup) {

    int res;
    auto opt1 = app.add_option("--test1", res);
    auto opt2 = app.add_option("--test2", res);
    auto opt3 = app.add_option("--test3", res);
    int val2;
    app.add_option("--option", val2);

    auto ogroup = app.add_option_group("clusters");
    ogroup->require_option();
    ogroup->add_options(opt1, opt2, opt3);

    EXPECT_THROW(ogroup->add_options(opt1), CLI::OptionNotFound);
    args = {"--option", "9"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--test1", "5", "--test2", "4", "--test3=5"};
    EXPECT_NO_THROW(run());

    std::string help = app.help();
    auto exactloc = help.find("[At least 1");
    auto oloc = help.find("--test1");
    EXPECT_NE(exactloc, std::string::npos);
    EXPECT_NE(oloc, std::string::npos);
    EXPECT_LT(exactloc, oloc);
}

TEST_F(TApp, BasicOptionGroupAddFailures) {

    int res;
    auto opt1 = app.add_option("--test1", res);
    app.set_config("--config");
    int val2;
    app.add_option("--option", val2);

    auto ogroup = app.add_option_group("clusters");
    EXPECT_THROW(ogroup->add_options(app.get_config_ptr()), CLI::OptionAlreadyAdded);
    EXPECT_THROW(ogroup->add_options(app.get_help_ptr()), CLI::OptionAlreadyAdded);

    auto sub = app.add_subcommand("sub", "subcommand");
    auto opt2 = sub->add_option("--option2", val2);

    EXPECT_THROW(ogroup->add_option(opt2), CLI::OptionNotFound);

    EXPECT_THROW(ogroup->add_options(nullptr), CLI::OptionNotFound);

    ogroup->add_option(opt1);

    auto opt3 = app.add_option("--test1", res);

    EXPECT_THROW(ogroup->add_option(opt3), CLI::OptionAlreadyAdded);
}

TEST_F(TApp, BasicOptionGroupScrewedUpMove) {

    int res;
    auto opt1 = app.add_option("--test1", res);
    auto opt2 = app.add_option("--test2", res);
    int val2;
    app.add_option("--option", val2);

    auto ogroup = app.add_option_group("clusters");
    ogroup->require_option();
    auto ogroup2 = ogroup->add_option_group("clusters2");
    EXPECT_THROW(ogroup2->add_options(opt1, opt2), CLI::OptionNotFound);

    CLI::Option_group EmptyGroup("description", "new group", nullptr);

    EXPECT_THROW(EmptyGroup.add_option(opt2), CLI::OptionNotFound);
    EXPECT_THROW(app._move_option(opt2, ogroup2), CLI::OptionNotFound);
}

TEST_F(TApp, InvalidOptions) {
    auto ogroup = app.add_option_group("clusters");
    CLI::Option *opt = nullptr;
    EXPECT_THROW(ogroup->excludes(opt), CLI::OptionNotFound);
    CLI::App *app_p = nullptr;
    EXPECT_THROW(ogroup->excludes(app_p), CLI::OptionNotFound);
    EXPECT_THROW(ogroup->excludes(ogroup), CLI::OptionNotFound);
    EXPECT_THROW(ogroup->add_option(opt), CLI::OptionNotFound);
}

struct ManyGroups : public TApp {

    CLI::Option_group *main;
    CLI::Option_group *g1;
    CLI::Option_group *g2;
    CLI::Option_group *g3;
    std::string name1;
    std::string name2;
    std::string name3;
    std::string val1;
    std::string val2;
    std::string val3;
    ManyGroups() {
        main = app.add_option_group("main", "the main outer group");
        g1 = main->add_option_group("g1", "group1 description");
        g2 = main->add_option_group("g2", "group2 description");
        g3 = main->add_option_group("g3", "group3 description");
        g1->add_option("--name1", name1)->required();
        g1->add_option("--val1", val1);
        g2->add_option("--name2", name2)->required();
        g2->add_option("--val2", val2);
        g3->add_option("--name3", name3)->required();
        g3->add_option("--val3", val3);
    }

    void remove_required() {
        g1->get_option("--name1")->required(false);
        g2->get_option("--name2")->required(false);
        g3->get_option("--name3")->required(false);
        g1->required(false);
        g2->required(false);
        g3->required(false);
    }
};

TEST_F(ManyGroups, SingleGroup) {
    // only 1 group can be used
    main->require_option(1);
    args = {"--name1", "test"};
    run();
    EXPECT_EQ(name1, "test");

    args = {"--name2", "test", "--val2", "tval"};

    run();
    EXPECT_EQ(val2, "tval");

    args = {"--name1", "test", "--val2", "tval"};

    EXPECT_THROW(run(), CLI::RequiredError);
}

TEST_F(ManyGroups, SingleGroupError) {
    // only 1 group can be used
    main->require_option(1);
    args = {"--name1", "test", "--name2", "test3"};
    EXPECT_THROW(run(), CLI::RequiredError);
}

TEST_F(ManyGroups, AtMostOneGroup) {
    // only 1 group can be used
    main->require_option(0, 1);
    args = {"--name1", "test", "--name2", "test3"};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {};
    EXPECT_NO_THROW(run());
}

TEST_F(ManyGroups, AtLeastTwoGroups) {
    // only 1 group can be used
    main->require_option(2, 0);
    args = {"--name1", "test", "--name2", "test3"};
    run();

    args = {"--name1", "test"};
    EXPECT_THROW(run(), CLI::RequiredError);
}

TEST_F(ManyGroups, BetweenOneAndTwoGroups) {
    // only 1 group can be used
    main->require_option(1, 2);
    args = {"--name1", "test", "--name2", "test3"};
    run();

    args = {"--name1", "test"};
    run();

    args = {};
    EXPECT_THROW(run(), CLI::RequiredError);

    args = {"--name1", "test", "--name2", "test3", "--name3=test3"};
    EXPECT_THROW(run(), CLI::RequiredError);
}

TEST_F(ManyGroups, RequiredFirst) {
    // only 1 group can be used
    remove_required();
    g1->required();

    EXPECT_TRUE(g1->get_required());
    EXPECT_FALSE(g2->get_required());
    args = {"--name1", "test", "--name2", "test3"};
    run();

    args = {"--name2", "test"};
    try {
        run();
    } catch(const CLI::RequiredError &re) {
        EXPECT_THAT(re.what(), HasSubstr("g1"));
    }

    args = {"--name1", "test", "--name2", "test3", "--name3=test3"};
    EXPECT_NO_THROW(run());
}

TEST_F(ManyGroups, DisableFirst) {
    // only 1 group can be used
    remove_required();
    g1->disabled();

    EXPECT_TRUE(g1->get_disabled());
    EXPECT_FALSE(g2->get_disabled());
    args = {"--name2", "test"};

    run();

    args = {"--name1", "test", "--name2", "test3"};
    EXPECT_THROW(run(), CLI::ExtrasError);
    g1->disabled(false);
    args = {"--name1", "test", "--name2", "test3", "--name3=test3"};
    EXPECT_NO_THROW(run());
}

TEST_F(ManyGroups, SameSubcommand) {
    // only 1 group can be used
    remove_required();
    auto sub1 = g1->add_subcommand("sub1");
    auto sub2 = g2->add_subcommand("sub1");
    auto sub3 = g3->add_subcommand("sub1");

    args = {"sub1", "sub1", "sub1"};

    run();

    EXPECT_TRUE(*sub1);
    EXPECT_TRUE(*sub2);
    EXPECT_TRUE(*sub3);
    /// This should be made to work at some point
    /*auto subs = app.get_subcommands();
    EXPECT_EQ(subs.size(), 3u);
    EXPECT_EQ(subs[0], sub1);
    EXPECT_EQ(subs[1], sub2);
    EXPECT_EQ(subs[2], sub3);
    */
    args = {"sub1", "sub1", "sub1", "sub1"};
    // for the 4th and future ones they will route to the first one
    run();
    EXPECT_EQ(sub1->count(), 2u);
    EXPECT_EQ(sub2->count(), 1u);
    EXPECT_EQ(sub3->count(), 1u);

    // subs should remain the same since the duplicate would not be registered there
    /*subs = app.get_subcommands();
    EXPECT_EQ(subs.size(), 3u);
    EXPECT_EQ(subs[0], sub1);
    EXPECT_EQ(subs[1], sub2);
    EXPECT_EQ(subs[2], sub3);*/
}
