#pragma once

enum class AstNodeAcceptStatus {
	GOTO_CHILD,  // A new AST child node was added, go to it
	GOTO_PARENT, // The AST node desn't accept tokens anymore, go to its parent
	STAY         // Keep processing the current AST node in stack
};
