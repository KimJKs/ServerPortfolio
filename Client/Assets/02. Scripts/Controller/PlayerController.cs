using Protocol;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class PlayerController : CreatureController
{
	protected Coroutine _coSkill;
	protected bool _rangedSkill = false;

	public override void Init()
	{
		base.Init();

		_controller.enabled = true;

		//채팅 UI 추가
		Managers.UI.AddUI($"Overhead_Dialogue_{this.Id}", transform.Find("Canvas").Find("DialogueBG").gameObject);

	}
	public virtual void SetNameText(string name)
	{
		if (string.IsNullOrEmpty(name))
			return;

		TextMeshProUGUI text = transform.Find("Canvas").Find("NameText").GetComponent<TextMeshProUGUI>();
		text.gameObject.SetActive(true);
		text.text = name;
	}

	protected override void UpdateAnimation(CreatureState previousState)
	{
		if (_animator == null)
			return;



		switch (State)
		{
			case CreatureState.Idle:
				//   _animator.Play("Idle");
				_animator.SetBool("isMoving", false);
				break;
			case CreatureState.Moving:
				//_animator.Play("Move");
				_animator.SetBool("isMoving", true);
				break;
			case CreatureState.Attack:
				//     _animator.Play("Attack");
				_animator.SetBool("isMoving", false);
				_animator.SetTrigger("isAttack");

				Invoke("BackToIdle", 0.5f);
				break;
			case CreatureState.Dead:
				_animator.Play("Die");
				break;
		}
	}

	protected override void UpdateController()
	{
		base.UpdateController();
	}

	protected override void MoveToTarget()
	{
		// XZ축만 고려하도록 Y값 고정
		Vector3 currentPositionXZ = new Vector3(transform.position.x, 0, transform.position.z);
		Vector3 targetPositionXZ = new Vector3(_targetPosition.x, 0, _targetPosition.z);


		Vector3 moveDirection = (targetPositionXZ - currentPositionXZ).normalized;
		_controller.Move(moveDirection * Stat.Speed * Time.deltaTime);

		// XZ축 회전 처리
		Quaternion targetRotation = Quaternion.LookRotation(moveDirection);
		transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, Time.deltaTime * 10);

	}

	//공격 이후 원상태로 돌리기
	public void BackToIdle()
	{
		State = CreatureState.Idle;
	}

	public override void OnDamaged()
	{
		Debug.Log("Player HIT !");
	}
}
