using Protocol;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MonsterController : CreatureController
{
	Coroutine _coSkill;

	void Start()
	{
	}

	private void OnEnable()
	{
		//체력바 초기화
		Managers.UI.UpdateHealthBar(this.Id, 1, 1);
	}

	public override void Init()
	{
		base.Init();

		//체력바 UI추가
		Managers.UI.AddUI($"HealthBar_{this.Id}", transform.Find("Canvas").Find("HPBG").gameObject);
	}

	protected override void UpdateIdle()
	{
		base.UpdateIdle();
	}

	protected override void UpdateAnimation(CreatureState previousState)
	{
		if (_animator == null)
			return;

		switch (State)
		{
			case CreatureState.Attack:
				_animator.SetTrigger("Jump");
				break;
			case CreatureState.Dead:
				_animator.Play("Die");
				break;
		}
	}

	public override void OnDamaged()
	{
	}

	public override void Attack(float directionX, float directionZ)
	{
		//만약 이미 공격상태라면 공격 애니메이션 다시 재생
		if (State == CreatureState.Attack) _animator.Play("Jump");

		base.Attack(directionX, directionZ);
	}

	public override void OnDead()
	{
		base.OnDead();

		Invoke("DestroyMonster", 1f);
	}

	void DestroyMonster()
	{

		Managers.Object.Remove(Id);
	}


}
