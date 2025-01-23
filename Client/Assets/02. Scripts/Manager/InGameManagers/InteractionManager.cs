using System.Collections.Generic;
using UnityEngine;


public interface IInteractable
{
    void Interact();
}

public class InteractionManager
{
    private IInteractable _currentInteractable;

    public void Register(IInteractable interactable)
    {
        if (_currentInteractable == null)
        {
            _currentInteractable = interactable;
            Managers.UI.SetActiveInteractionNoti(true);
        }
    }

    public void Unregister(IInteractable interactable)
    {
        if (_currentInteractable == interactable)
        {
            _currentInteractable = null;
            Managers.UI.SetActiveInteractionNoti(false);
        }
    }

    public void CheckInteraction()
    {
        if (_currentInteractable != null)
        {
            _currentInteractable.Interact();
        }
    }
}
