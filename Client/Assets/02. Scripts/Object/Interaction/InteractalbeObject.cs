using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InteractalbeObject : MonoBehaviour, IInteractable
{
    private void OnTriggerEnter(Collider other)
    {
        var player = other.GetComponent<MyPlayerController>();
        if (player != null)
        {
            Managers.Interaction.Register(this);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        var player = other.GetComponent<MyPlayerController>();
        if (player != null)
        {
            Managers.Interaction.Unregister(this);
        }
    }

    private void OnDisable()
    {
        var player = FindObjectOfType<MyPlayerController>();
        if (player != null)
        {
            Managers.Interaction.Unregister(this);
        }
    }

    public virtual void Interact()
    {

    }
}
