using System;
using System.Collections.Generic;
using Protocol;
using UnityEngine;

public class InputManager
{
    private Dictionary<KeyCode, Action> _keyBindings = new Dictionary<KeyCode, Action>();

    public void Init()
    {
        BindKey(KeyCode.Tab, () => { Managers.UI.ActivePopInventory(); });

        BindKey(KeyCode.Escape, Managers.UI.InActivePop);

        BindKey(KeyCode.E, Managers.Interaction.CheckInteraction);

        BindKey(KeyCode.F1, () => { Managers.Skill.UseSkill(1); });

        BindKey(KeyCode.F2, () => { Managers.Skill.UseSkill(2); });

    }

    public void Update()
    {
        // 매 프레임 키 입력 확인
        foreach (var keyBinding in _keyBindings)
        {
            if (Input.GetKeyDown(keyBinding.Key))
            {
                keyBinding.Value?.Invoke();
            }
        }
    }

    public void BindKey(KeyCode key, Action action)
    {
        if (_keyBindings.ContainsKey(key))
        {
            _keyBindings[key] = action;
        }
        else
        {
            _keyBindings.Add(key, action);
        }
    }

    public void UnbindKey(KeyCode key)
    {
        if (_keyBindings.ContainsKey(key))
        {
            _keyBindings.Remove(key);
        }
    }


}
