#ifndef HISTORY_HPP
    #define HISTORY_HPP

    #include <cstddef>
    #include <cstdint>
    #include <deque>
    #include <vector>

    #include "document.hpp"

namespace gimp {

class History {
    struct LayerState {
        std::vector<uint8_t> Buffer;
        std::vector<uint8_t> Mask;
    };

    struct Entry {
        int LayerIndex = -1;
        std::vector<uint8_t> Buffer;
        std::vector<uint8_t> Mask;
        bool IsResize = false;
        int Width = 0;
        int Height = 0;
        std::vector<LayerState> Layers;
    };

    Document& Doc;
    std::size_t Depth;
    std::deque<Entry> UndoStack;
    std::deque<Entry> RedoStack;

    void Push(Entry&& entry);
    void Apply(Entry& entry);
    void ApplyResize(Entry& entry);

public:
    History(Document& document, std::size_t depth);

    void RecordPixels(int layerIndex);
    void RecordResize();
    void ForgetLayer(int layerIndex);

    void Undo();
    void Redo();

    bool canUndo() const {return !UndoStack.empty();};
    bool canRedo() const {return !RedoStack.empty();};
};

}

#endif
