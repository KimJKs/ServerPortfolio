using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Market : InteractalbeObject
{

    public override void Interact()
    {
        Managers.UI.ActivePopMarket();
    }
}
