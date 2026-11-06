#include "history.hpp"

#include <algorithm>

namespace gimp {

History::History(Document& document, std::size_t depth) : Doc(document), Depth(depth) {}

void History::Push(Entry&& entry) {
    UndoStack.push_back(std::move(entry));
    if (UndoStack.size() > Depth)
        UndoStack.pop_front();
    RedoStack.clear();
}

void History::ApplyResize(Entry& entry) {
    const int width = Doc.getWidth();
    const int height = Doc.getHeight();

    for (int i = 0; i < Doc.getLayerCount(); i++) {
        Layer& layer = Doc.getLayer(i);
        if (i >= static_cast<int>(entry.Layers.size()))
            entry.Layers.emplace_back();
        LayerState& state = entry.Layers[i];

        std::vector<uint8_t> buffer = layer.getBuffer();
        std::vector<uint8_t> mask = layer.getMask();
        if (state.Buffer.empty()) {
            layer.Resize(entry.Width, entry.Height, false);
        } else {
            layer.setSize(entry.Width, entry.Height);
            layer.setBuffer(state.Buffer);
            layer.setMask(state.Mask);
        }
        state.Buffer = std::move(buffer);
        state.Mask = std::move(mask);
    }

    Doc.setSize(entry.Width, entry.Height);
    entry.Width = width;
    entry.Height = height;
}

void History::Apply(Entry& entry) {
    if (entry.IsResize) {
        ApplyResize(entry);
        return;
    }

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

    Push(std::move(entry));
}

void History::RecordResize() {
    Entry entry;
    entry.IsResize = true;
    entry.Width = Doc.getWidth();
    entry.Height = Doc.getHeight();
    for (int i = 0; i < Doc.getLayerCount(); i++)
        entry.Layers.push_back({Doc.getLayer(i).getBuffer(), Doc.getLayer(i).getMask()});

    Push(std::move(entry));
}

void History::ForgetLayer(int layerIndex) {
    for (std::deque<Entry>* stack : {&UndoStack, &RedoStack}) {
        stack->erase(std::remove_if(stack->begin(), stack->end(),
            [layerIndex](const Entry& entry) { return entry.LayerIndex == layerIndex; }), stack->end());
        for (Entry& entry : *stack) {
            if (entry.LayerIndex > layerIndex)
                entry.LayerIndex--;
            if (entry.IsResize && layerIndex < static_cast<int>(entry.Layers.size()))
                entry.Layers.erase(entry.Layers.begin() + layerIndex);
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
