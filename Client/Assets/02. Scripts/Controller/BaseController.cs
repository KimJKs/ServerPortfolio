using Protocol;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using UnityEngine;

public class BaseController : MonoBehaviour
{
	public int Id { get; set; }

	StatInfo _stat = new StatInfo();
	public virtual StatInfo Stat
	{
		get { return _stat; }
		set
		{
			if (_stat.Equals(value))
				return;

			_stat.MergeFrom(value);
		}
	}

	public float Speed
	{
		get { return Stat.Speed; }
		set { Stat.Speed = value; }
	}

	public virtual int Hp
	{
		get { return Stat.Hp; }
		set
		{
			Stat.Hp = value;
		}
	}

	public virtual int Mp
	{
		get { return Stat.Mp; }
		set
		{
			Stat.Mp = value;
		}
	}


	protected bool _updated = false;

	PositionInfo _positionInfo = new PositionInfo();

	public PositionInfo PosInfo
	{
		get { return _positionInfo; }
		set
		{
			if (_positionInfo.Equals(value))
				return;

			_targetPosition = new Vector3(value.PosX, this.transform.position.y, value.PosZ);

			State = value.State;

		}
	}

	protected Vector3 _targetPosition = new Vector3();

	protected Animator _animator;

	public virtual CreatureState State
	{
		get { return PosInfo.State; }
		set
		{
			if (PosInfo.State == value)
				return;
			CreatureState previousState = PosInfo.State;
			PosInfo.State = value;
			UpdateAnimation(previousState);
			_updated = true;
		}
	}

	protected virtual void UpdateAnimation(CreatureState previousState)
	{

	}

	private void Awake()
	{

	}

	void Start()
	{
		Init();
	}

	private void Update()
	{
		UpdateController();
	}


	public virtual void Init()
	{
		_animator = GetComponent<Animator>();

		UpdateAnimation(CreatureState.Idle);


	}

	protected virtual void UpdateController()
	{

		switch (State)
		{
			case CreatureState.Idle:
				UpdateIdle();
				break;
			case CreatureState.Moving:
				UpdateMoving();
				break;
			case CreatureState.Attack:
				UpdateAttack();
				break;
			case CreatureState.Dead:
				UpdateDead();
				break;
		}
	}

	protected virtual void UpdateIdle()
	{
	}

	// 스르륵 이동하는 것을 처리
	protected virtual void UpdateMoving()
	{
		// xz 평면 거리 계산
		Vector2 currentXZ = new Vector2(this.transform.position.x, this.transform.position.z);
		Vector2 targetXZ = new Vector2(_targetPosition.x, _targetPosition.z);

		float distance = Vector2.Distance(currentXZ, targetXZ);

		if (distance > 0.1f)
		{

			MoveToTarget();
		}
		else
		{
			State = CreatureState.Idle;
		}
	}

	protected virtual void MoveToNextPos()
	{

	}

	protected virtual void UpdateAttack()
	{

	}

	protected virtual void UpdateDead()
	{

	}

	protected virtual void MoveToTarget()
	{

	}
}
