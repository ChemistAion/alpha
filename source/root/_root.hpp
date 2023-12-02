#pragma once

#include "core/_core.hpp"
#include "ecs/_ecs.hpp"
#include "ecs/registry.hpp"
#include "engine/_engine.hpp"

//events
#include "event/tick.hpp"
#include "event/tock.hpp"
#include "event/process.hpp"
#include "event/terminate.hpp"

#include "event/task.hpp"
#include "event/touch.hpp"
#include "event/frame.hpp"
#include "event/create.hpp"
#include "event/resize.hpp"

//components
#include "component/volume.hpp"
#include "component/file.hpp"
#include "component/editor.hpp"

//systems
#include "system/worker.hpp"
#include "system/storage.hpp"
#include "system/engine.hpp"