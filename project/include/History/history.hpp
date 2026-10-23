#ifndef HISTORY_HPP
    #define HISTORY_HPP

    #include <cstddef>
    #include <cstdint>
    #include <deque>
    #include <vector>

    #include "document.hpp"

namespace gimp {

class History {
    struct Entry {
        int LayerIndex = -1;
        std::vector<uint8_t> Buffer;
        std::vector<uint8_t> Mask;
    };

    Document& Doc;
    std::size_t Depth;
    std::deque<Entry> UndoStack;
    std::deque<Entry> RedoStack;

    void Apply(Entry& entry);

public:
    History(Document& document, std::size_t depth);

    void RecordPixels(int layerIndex);
    void ForgetLayer(int layerIndex);

    void Undo();
    void Redo();

    bool canUndo() const {return !UndoStack.empty();};
    bool canRedo() const {return !RedoStack.empty();};
};

}

#endif
