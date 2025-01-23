using Protocol;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class MyPlayerController : PlayerController
{
	bool _moveKeyPressed = false;

	public int WeaponDamage { get; private set; }
	public int ArmorDefence { get; private set; }

	public string Name;

	public int GoldAmount = 0;

	private Vector3 _lastSentPosition = Vector3.zero;   // 마지막으로 전송한 위치
	private float _lastSentTime = 0f;                  // 마지막 패킷 전송 시간
	private float _posSyncInterval = 1f;                // 위치 동기화 패킷 전송 최소 간격 (초 단위)
	private float _posSyncDist = 3f;                // 위치 동기화 패킷 전송 최소 거리

	public override void Init()
	{
		base.Init();

	}

	protected override void UpdateController()
	{
		GetUIKeyInput();

		switch (State)
		{
			case CreatureState.Idle:
				HandleMouseInput();
				break;
			case CreatureState.Moving:
				HandleMouseInput();
				SendPosPacket();
				break;
		}

		base.UpdateController();
	}
	protected override void UpdateAnimation(CreatureState previousState)
	{
		base.UpdateAnimation(previousState);

		if (previousState == CreatureState.Moving)
		{
			PositionInfo pos = new PositionInfo
			{
				PosX = this.transform.position.x,
				PosZ = this.transform.position.z,
				State = CreatureState.Moving
			};
			C_POS_SYNC posSyncPacket = new C_POS_SYNC
			{
				PosInfo = pos,
			};

			Managers.Network.Send(posSyncPacket);  // 서버로 위치 패킷 전송

		}
	}

	protected override void UpdateIdle()
	{

	}

	Coroutine _coSkillCooltime;
	IEnumerator CoInputCooltime(float time)
	{
		yield return new WaitForSeconds(time);
		_coSkillCooltime = null;
	}


	// 마우스 입력 처리 (Plane 태그가 있는 곳 클릭 시 서버에 이동 요청)
	void HandleMouseInput()
	{

		// UI 클릭 무시
		if (EventSystem.current.IsPointerOverGameObject())
			return;


		if (Input.GetMouseButtonDown(1)) // 마우스 오른쪽 클릭
		{
			Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
			RaycastHit hit;

			if (Physics.Raycast(ray, out hit))
			{
				// Plane 태그가 있는 오브젝트만 이동
				if (hit.collider.CompareTag("Plane"))
				{
					Vector3 clickPosition = hit.point;
					clickPosition.y = transform.position.y;  // 현재 캐릭터의 Y값 유지
					PositionInfo pos = new PositionInfo
					{
						PosX = clickPosition.x,
						PosZ = clickPosition.z,
						State = CreatureState.Moving
					};

					C_MOVE movePacket = new C_MOVE
					{
						PosInfo = pos,
						TimeStamp = (ulong)(DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalMilliseconds
					};
					Managers.Network.Send(movePacket);  // 서버로 이동 요청 패킷 전송
				}
			}
		}

		if (Input.GetMouseButtonDown(0)) // 마우스 왼쪽 클릭 - 공격
		{
			Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
			RaycastHit hit;

			if (Physics.Raycast(ray, out hit))
			{
				// Plane 태그가 있는 오브젝트만 공격 처리
				if (hit.collider.CompareTag("Plane"))
				{
					Vector3 clickPosition = hit.point;
					Vector3 attackDirection = (clickPosition - transform.position).normalized;

					// 공격 요청 패킷 전송
					C_ATTACK attackPacket = new C_ATTACK
					{
						DirX = attackDirection.x,
						DirZ = attackDirection.z
					};
					Managers.Network.Send(attackPacket);  // 서버로 공격 요청 패킷 전송

				}
			}
		}
	}


	void LateUpdate()
	{
		// 목표 카메라 위치 설정 (플레이어의 위치 기준)
		Vector3 desiredCameraPosition = transform.position + new Vector3(0, 5, -2f);

		// 현재 카메라 위치에서 목표 위치로 스무스하게 이동
		Camera.main.transform.position = Vector3.Lerp(Camera.main.transform.position, desiredCameraPosition, Time.deltaTime * 10);
	}

	void GetUIKeyInput()
	{

	}


	// 위치 패킷 전송
	public void SendPosPacket()
	{
		// 현재 시간
		float currentTime = Time.time;

		// xz 평면 거리 계산
		Vector2 currentXZ = new Vector2(this.transform.position.x, this.transform.position.z);
		Vector2 targetXZ = new Vector2(_lastSentPosition.x, _lastSentPosition.z);

		float distance = Vector2.Distance(currentXZ, targetXZ);

		// 마지막 전송한 위치에서 일정 거리 이상 이동하지 않았거나 일정 시간이 지나지 않았을 경우 전송하지 않음
		if (distance <= _posSyncDist || currentTime - _lastSentTime < _posSyncInterval)
			return;

		// 위치 패킷 전송
		PositionInfo pos = new PositionInfo
		{
			PosX = this.transform.position.x,
			PosZ = this.transform.position.z,
			State = CreatureState.Moving
		};

		C_POS_SYNC posSyncPacket = new C_POS_SYNC
		{
			PosInfo = pos,
		};

		Managers.Network.Send(posSyncPacket);  // 서버로 위치 패킷 전송

		_lastSentPosition = transform.position;
		_lastSentTime = currentTime; // 마지막 패킷 전송 시간 갱신
	}

	protected override void MoveToTarget()
	{
		base.MoveToTarget();
		// XZ축만 고려
		Vector3 currentPositionXZ = new Vector3(transform.position.x, 0, transform.position.z);
		Vector3 targetPositionXZ = new Vector3(_targetPosition.x, 0, _targetPosition.z);

		// 목표 지점에 도달 여부 확인
		if (Vector3.Distance(currentPositionXZ, targetPositionXZ) <= 0.1f)
		{
			ReachTarget();
		}
	}

	protected override void ReachTarget()
	{
		base.ReachTarget();

	}



	/*
		public void RefreshAdditionalStat()
		{
			WeaponDamage = 0;
			ArmorDefence = 0;

			foreach (Item item in Managers.Inven.Items.Values)
			{
				if (item.Equipped == false)
					continue;

				switch (item.ItemType)
				{
					case ItemType.Weapon:
						WeaponDamage += ((Weapon)item).Damage;
						break;
					case ItemType.Armor:
						ArmorDefence += ((Armor)item).Defence;
						break;
				}
			}
		}
		*/
}
