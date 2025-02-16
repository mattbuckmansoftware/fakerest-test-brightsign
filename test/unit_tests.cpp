#include <gtest/gtest.h>
#include "../src/data_handler.h"

TEST(DataHandlerTest, Parse_AllGood) {
    std::string read_buffer = "{\"id\":0,\"name\":\"Eli\",\"city\":\"Austin\",\"age\":78,\"friends\":[{\"name\":\"Robert\",\"hobbies\":[\"Travel\"]}]}\n\
{\"id\":1,\"name\":\"Eli\",\"city\":\"Boston\",\"age\":97,\"friends\":[]}\n\
{\"id\":2,\"name\":\"Evy\",\"city\":\"San Diego\",\"age\":48,\"friends\":[{\"name\":\"Joe\",\"hobbies\":[\"Reading\"]}]}";
    
    DataHandler handler{};
    EXPECT_EQ(handler.parseJSON(read_buffer), 3);
}

TEST(DataHandlerTest, Parse_OneBad) {
    std::string read_buffer = "{\"id\":0,\"name\":\"Eli\",\"city\":\"Austin\",\"age\":78,\"friends\":[{\"name\":\"Robert\",\"hobbies\":[\"Travel\"]}]}\n\
{\"id\":1,\"name\":\"Noah\",\"city\":\"Boston\",\"age\":97,\"friends\":[]}{\n\
{\"id\":2,\"name\":\"Evy\",\"city\":\"San Diego\",\"age\":48,\"friends\":[{\"name\":\"Joe\",\"hobbies\":[\"Reading\"]}]}";
    
    DataHandler handler{};
    EXPECT_EQ(handler.parseJSON(read_buffer), 2);
}

TEST(DataHandlerTest, Parse_StringNumber) {
    std::string read_buffer = "{\"id\":0,\"name\":\"Eli\",\"city\":\"Austin\",\"age\":\"78\",\"friends\":[{\"name\":\"Robert\",\"hobbies\":[\"Travel\"]}]}";
    
    DataHandler handler{};
    EXPECT_EQ(handler.parseJSON(read_buffer), 1);
}

//TODO: Check output

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
