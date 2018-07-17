
using System.Collections.Generic;
using UnityEngine;

public class FSM {

    private Stack<FSMState> stateStack = new Stack<FSMState>();

    // Used to Invoke FSMState Contructor
    public delegate void FSMState(FSM fsm, GameObject gameObject);


    public void Update(GameObject gameObject)
    {
        // Peek returns the element at the Top of the Stack,
        // Without removing it
        if (stateStack.Peek() != null && AIManager.instance.GetActive())
            stateStack.Peek().Invoke(this, gameObject);
    }

    // Push State onto Stack
    public void Push(FSMState state) {
        stateStack.Push(state);
    }

    // Remove State from Stack
    public void Pop() {
        stateStack.Pop();
    }

    public void Clear()
    {
        stateStack.Clear();
    }
}
