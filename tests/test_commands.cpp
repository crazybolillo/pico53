#include "gtest/gtest.h"

extern "C" {
#include "commands.h"
}

class TestCommands : public testing::Test {};

TEST_F(TestCommands, GetCommand) {
    ASSERT_EQ(COMMAND_INIT, get_command("init 0x60"));
    ASSERT_EQ(COMMAND_WRITE, get_command("write 0x1A 0x0x00"));
}

TEST_F(TestCommands, InitCommand) {
    uint8_t address;

    ASSERT_EQ(0, command_init("init 0x70", &address));
    ASSERT_EQ(0x70, address);

    ASSERT_EQ(0, command_init("init kiwi", &address));
    ASSERT_EQ(0x60, address);

    address = 0x27;
    ASSERT_EQ(0, command_init("init", &address));
    ASSERT_EQ(0x60, address);
}

TEST_F(TestCommands, FreqCommand) {
    struct FreqCommand command = {.multisynth = MS0, .frequency = 0};

    ASSERT_EQ(0, command_set_freq("freq 1 30e6", &command));
    ASSERT_EQ(MS1, command.multisynth);
    ASSERT_EQ(30e6, command.frequency);
    ASSERT_EQ(0, command.invert);

    ASSERT_EQ(0, command_set_freq("freq 0 153.0125e6 inv", &command));
    ASSERT_EQ(MS0, command.multisynth);
    ASSERT_EQ(153.0125e6, command.frequency);
    ASSERT_EQ(1, command.invert);

    ASSERT_EQ(1, command_set_freq("freq 32 30e6", &command));
    ASSERT_EQ(1, command_set_freq("freq 0", &command));
}

TEST_F(TestCommands, ReadCommand) {
    uint8_t address;

    ASSERT_EQ(0, command_read("read 0x1A", &address));
    ASSERT_EQ(1, command_read("read", &address));
    ASSERT_EQ(1, command_read("read hello", &address));
}

TEST_F(TestCommands, WriteCommand) {
    struct WriteCommand command = {0};

    ASSERT_EQ(0, command_write("write 0x1A 0xFF", &command));
    ASSERT_EQ(0x1A, command.reg);
    ASSERT_EQ(0xFF, command.value);

    ASSERT_EQ(1, command_write("write", &command));
}

TEST_F(TestCommands, EnableCommand) {
    uint8_t ms = 0;

    ASSERT_EQ(0, command_enable("enable 2", &ms));
    ASSERT_EQ(2, ms);

    ASSERT_EQ(1, command_enable("enable", &ms));
    ASSERT_EQ(1, command_enable("enable hello", &ms));
}

TEST_F(TestCommands, DisableCommand) {
    uint8_t ms = 0;

    ASSERT_EQ(0, command_disable("disable 2", &ms));
    ASSERT_EQ(2, ms);

    ASSERT_EQ(1, command_disable("disable", &ms));
    ASSERT_EQ(1, command_disable("disable hello", &ms));
}
