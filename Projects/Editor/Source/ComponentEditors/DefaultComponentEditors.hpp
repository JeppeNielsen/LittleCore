//
// Created by Jeppe Nielsen on 25/12/2024.
//

#pragma once
#include "ComponentEditorCollection.hpp"
#include "Editors/LocalBoundingBoxEditor.hpp"
#include "Editors/LocalTransformEditor.hpp"
#include "Editors/CameraEditor.hpp"
#include "Editors/TexturableEditor.hpp"

using DefaultComponentEditors =
        ComponentEditorCollection<
                LocalBoundingBoxEditor,
                LocalTransformEditor,
                CameraEditor,
                TexturableEditor
                >;

template<typename ...T>
using CustomComponentEditors = ConcatComponentEditorCollections<DefaultComponentEditors, ComponentEditorCollection<T...>>;
