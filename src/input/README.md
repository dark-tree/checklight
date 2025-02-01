## Input System

To use the input system include the provided `input.hpp` header file, it includes all the other public headers.
The input system contains 3 important classes:
 - `InputDispatcher` an event dispatcher, to receive events from the system register your code with the nearest event input dispatcher.
The root `InputDispatcher` can be obtained from the `Window` class (part of render system) using `getInputDispatcher()`.
 - `InputListener` an abstract class that has a `onEvent()` method, you need to implement it and register in the aforementioned input dispatcher.
For debugging a `DebugInputListener` is provided, when registered will log all (except for `MouseEvent`) received input events.
 - `InputEvent` an class from which all other input events derive.

### Example

Registering `InputListener`s
```c++
// include input system
#include "input/input.hpp"

int main() {
	
	// We assume you already have a reference to the InputDispatcher here 
	// You can access root InputDispatcher from RenderSystem::system->getWindow().getInputDispatcher()
	InputDispatcher& dispatcher = /* ... */;

	// first register the DebugInputListener to log all input (except for mouse movement)
	dispatcher.registerListener(std::make_shared<DebugInputListener>());
	
	// then register your own listener (impl shown in the next example)
	dispatcher.registerListener(std::make_shared<MyInputListener>());
	
	// you can also remove a listener from a dispatcher
	// dispatcher.registerListener(/* shared_ptr */);
	
}

```

Writing an `InputListener`
```c++
class MyInputListener : public InputListener {
		
	public:
		
		InputResult onEvent(const InputEvent &event) override {
			
			// check & assert the event to a KeyboardEvent
			if (const auto* event = any.as<KeyboardEvent>()) {
				
				// if user pressed CTRL+Q
				if (event->wasPressed(GLFW_KEY_Q) && event->isCtrlPressed()) {
					printf("DEBUG: Hello World!\n");
					
					// cancel further event processing
					return InputResult::CONSUMED;
				}
			}
			
			// ignore all other events
			return InputResult::PASS;
		}
		
};
```