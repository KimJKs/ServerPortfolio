using System.Collections;
using System.Collections.Generic;
using Protocol;
using Unity.VisualScripting;
using UnityEngine;

public class SkillManager
{
    public Dictionary<int, bool> CanUseSkill = new Dictionary<int, bool>();

    public void UseSkill(int skillTemplateId)
    {
        if (CanUseSkill.TryGetValue(skillTemplateId, out bool canUseSkill) && !canUseSkill)
            return;

        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;
        Vector3 clickPosition = new Vector3();
        if (Physics.Raycast(ray, out hit))
        {
            // Plane 태그가 있는 오브젝트만 이동
            if (hit.collider.CompareTag("Plane"))
            {
                clickPosition = hit.point;

                PositionInfo pos = new PositionInfo
                {
                    PosX = clickPosition.x,
                    PosZ = clickPosition.z,
                    State = CreatureState.Moving
                };

                C_SKILL skillPkt = new C_SKILL();
                skillPkt.SkillTemplateId = skillTemplateId;
                skillPkt.PosInfo = pos;
                Managers.Network.Send(skillPkt);
            }
        }

    }

    public void ShowSkill(S_SKILL pkt)
    {
        Data.SkillData skillData = Managers.Data.SkillDict[pkt.SkillTemplateId];

        if (!Managers.Object.Has(pkt.AttackerId))
        {
            return;
        }

        ParticleSystem skillEffect = Managers.Resource.Instantiate(skillData.prefab).GetComponent<ParticleSystem>();
        skillEffect.transform.position = new Vector3(pkt.PosInfo.PosX, 0, pkt.PosInfo.PosZ);
        skillEffect.transform.rotation = Managers.Object.FindById(pkt.AttackerId).transform.rotation;
        skillEffect.gameObject.SetActive(true);


        if (pkt.AttackerId == Managers.Object.MyPlayer.Id)
        {
            Managers.UI.StartSkillCooltime(skillData.cooldown, skillData.id);

            CanUseSkill[skillData.id] = false;
        }
    }
}
