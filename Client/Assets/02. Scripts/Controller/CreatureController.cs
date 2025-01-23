using Protocol;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using UnityEngine;

public class CreatureController : BaseController
{
	protected CharacterController _controller;

	public override StatInfo Stat
	{
		get { return base.Stat; }
		set { base.Stat = value; }
	}

	public override int Hp
	{
		get { return Stat.Hp; }
		set { base.Hp = value; }
	}

	public override int Mp
	{
		get { return Stat.Mp; }
		set { base.Mp = value; }
	}



	public override void Init()
	{
		base.Init();
		_controller = this.GetComponent<CharacterController>();

	}

	public virtual void OnDamaged()
	{

	}

	public virtual void OnDead()
	{
		State = CreatureState.Dead;

	}

	public virtual void Attack(float directionX, float directionZ)
	{
		Vector3 attackDirection = new Vector3(directionX, 0, directionZ).normalized;

		Quaternion attackRotation = Quaternion.LookRotation(attackDirection);

		transform.rotation = attackRotation;

		State = CreatureState.Attack;

	}

	protected override void MoveToTarget()
	{
		// XZ축만 고려
		Vector3 currentPositionXZ = new Vector3(transform.position.x, -0.2f, transform.position.z);
		Vector3 targetPositionXZ = new Vector3(_targetPosition.x, -0.2f, _targetPosition.z);

		// 보간 처리
		transform.position = Vector3.Lerp(transform.position, targetPositionXZ, Time.deltaTime * Stat.Speed);

		// 회전 처리
		Vector3 moveDirection = (targetPositionXZ - currentPositionXZ).normalized;
		if (moveDirection != Vector3.zero)
		{
			Quaternion targetRotation = Quaternion.LookRotation(moveDirection);
			transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, Time.deltaTime * 10);
		}

		// 목표 지점에 도달 여부 확인
		if (Vector3.Distance(currentPositionXZ, targetPositionXZ) <= 0.1f)
		{
			ReachTarget();
		}
	}

	protected virtual void ReachTarget()
	{
		transform.position = _targetPosition; // 위치를 정확히 정렬
		State = CreatureState.Idle;
	}


}
