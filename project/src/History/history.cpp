#include "history.hpp"

#include <algorithm>

namespace gimp {

History::History(Document& document, std::size_t depth) : Doc(document), Depth(depth) {}

void History::Apply(Entry& entry) {
    Layer& layer = Doc.getLayer(entry.LayerIndex);
    std::vector<uint8_t> buffer = layer.getBuffer();
    std::vector<uint8_t> mask = layer.getMask();
    layer.setBuffer(entry.Buffer);
    layer.setMask(entry.Mask);
    entry.Buffer = std::move(buffer);
    entry.Mask = std::move(mask);
}

void History::RecordPixels(int layerIndex) {
    Layer& layer = Doc.getLayer(layerIndex);

    Entry entry;
    entry.LayerIndex = layerIndex;
    entry.Buffer = layer.getBuffer();
    entry.Mask = layer.getMask();

    UndoStack.push_back(std::move(entry));
    if (UndoStack.size() > Depth)
        UndoStack.pop_front();
    RedoStack.clear();
}

void History::ForgetLayer(int layerIndex) {
    for (std::deque<Entry>* stack : {&UndoStack, &RedoStack}) {
        stack->erase(std::remove_if(stack->begin(), stack->end(),
            [layerIndex](const Entry& entry) { return entry.LayerIndex == layerIndex; }), stack->end());
        for (Entry& entry : *stack) {
            if (entry.LayerIndex > layerIndex)
                entry.LayerIndex--;
        }
    }
}

void History::Undo() {
    if (UndoStack.empty())
        return;

    Entry entry = std::move(UndoStack.back());
    UndoStack.pop_back();
    Apply(entry);
    RedoStack.push_back(std::move(entry));
}

void History::Redo() {
    if (RedoStack.empty())
        return;

    Entry entry = std::move(RedoStack.back());
    RedoStack.pop_back();
    Apply(entry);
    UndoStack.push_back(std::move(entry));
}

}
