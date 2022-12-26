// Base class
class TextCommand {
protected: 
    std::string _args;

public:
    TextCommand(std::string args) {
        args.split
    }

    virtual run(GameState &state);
    void setWidth(int w) {
        width = w;
    }

    void setHeight(int h) {
        height = h;
    }


};