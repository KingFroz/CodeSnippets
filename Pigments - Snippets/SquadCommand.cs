using UnityEngine;
using System.Collections.Generic;
public class SquadCommand : MonoBehaviour {
    public GameObject viveCntrl;

    private SteamVR_TrackedObject trackedObject;
    private SteamVR_Controller.Device device { get { return SteamVR_Controller.Input((int)trackedObject.index); }  }

    [Header("Feedback UI")]
    public GameObject ui_Hold;
    public GameObject ui_Move;
    public GameObject ui_OnMe;
    public GameObject ui_Attack;

    public GameObject point;

    public enum Order { Regroup, Hold, Roam, Move, NOTOUCH };
    private Order m_Order;

    WeaponController m_WeaponController;
    RaycastHit rayInfo;

    public float lastTime = 0.0f;
    private const float waitTime = 15.0f;

    private AudioSource aSource;

    [Header("Sounds")]
    public AudioClip aClip;

    void Start () {
        trackedObject = viveCntrl.GetComponent<SteamVR_TrackedObject>();
        m_WeaponController = GetComponent<WeaponController>();

        rayInfo = new RaycastHit();
        m_Order = Order.NOTOUCH;

        aSource = GetComponent<AudioSource>();

        ui_OnMe.SetActive(false);
        ui_Hold.SetActive(false);
        ui_Attack.SetActive(false);
        ui_Move.SetActive(false);

        if (point == null) {
            point = GameObject.Find("Flag");
        }
    }

    void FixedUpdate () {
        if (m_WeaponController.GetState() == WeaponController.WeaponState.Command) {
            m_Order = CalculateTouch(device);
            Feedback();

            Vector3 fwd = m_WeaponController.GetMuzzlePosition().transform.TransformDirection(Vector3.forward);
            if (Physics.Raycast(m_WeaponController.GetMuzzlePosition().transform.position, fwd, out rayInfo, 100.0f))
            {
                point.transform.position = rayInfo.point;
            }
        } else {
            if(Time.time > (lastTime))
            {
                point.transform.position = new Vector3(0, -5, 0);
            }
        }
    }

    public Order GetOrder() {
        return m_Order;
    }

    public Vector3 GetPoint() {
        return rayInfo.point;
    }

    private void Feedback()
    {
        switch (m_Order)
        {
            case Order.Regroup:
                ui_OnMe.SetActive(true);
                ui_Hold.SetActive(false);
                ui_Attack.SetActive(false);
                ui_Move.SetActive(false);
                break;
            case Order.Hold:
                ui_OnMe.SetActive(false);
                ui_Hold.SetActive(true);
                ui_Attack.SetActive(false);
                ui_Move.SetActive(false);
                break;
            case Order.Roam:
                ui_OnMe.SetActive(false);
                ui_Hold.SetActive(false);
                ui_Attack.SetActive(true);
                ui_Move.SetActive(false);
                break;
            case Order.Move:
                ui_OnMe.SetActive(false);
                ui_Hold.SetActive(false);
                ui_Attack.SetActive(false);
                ui_Move.SetActive(true);
                break;
            case Order.NOTOUCH:
                ui_OnMe.SetActive(false);
                ui_Hold.SetActive(false);
                ui_Attack.SetActive(false);
                ui_Move.SetActive(false);
                break;
        }
    }

    private Order CalculateTouch(SteamVR_Controller.Device _device)
    {
        if (device.GetAxis().x > 0 && device.GetAxis().y > -0.5f && device.GetAxis().y < 0.5f && device.GetPress(SteamVR_Controller.ButtonMask.Touchpad)) {
            //Right
            return Order.Regroup;
        } else if (device.GetAxis().x < 0 && device.GetAxis().y > -0.5f && device.GetAxis().y < 0.5f && device.GetPress(SteamVR_Controller.ButtonMask.Touchpad)) {
            //Left
            return Order.Hold;
        } else if (device.GetAxis().y > 0 && device.GetAxis().x > -0.5f && device.GetAxis().x < 0.5f && device.GetPress(SteamVR_Controller.ButtonMask.Touchpad)) {
            //Up 
            return Order.Move;
        } else if (device.GetAxis().y < 0 && device.GetAxis().x > -0.5f && device.GetAxis().x < 0.5f && device.GetPress(SteamVR_Controller.ButtonMask.Touchpad)) {
            //Down
            return Order.Roam;
        }

        return Order.NOTOUCH;
    }

    public void MoveToPosition()
    {
        if (device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
        {
            int cap = GameManager.instance.GetTeamCount(GameManager.TeamEnum.Blue);
            for (int i = 1; i < cap; i++)
            {
                AI tempObject = GameManager.instance.GetTeamMemberAt(GameManager.TeamEnum.Blue, i).GetComponent<AI>();
                if (tempObject != null)
                {
                    tempObject.SquadBehaviorActive(true);
                    tempObject.RallyPosition(GetPoint());
                }
            }

            lastTime = Time.time + waitTime;
            aSource.PlayOneShot(aClip);
            m_WeaponController.SetState(WeaponController.WeaponState.Weapon);
        }
    }

    public void ReGroup()
    {
        if (device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
        {
            int cap = GameManager.instance.GetTeamCount(GameManager.TeamEnum.Blue);
            for (int i = 1; i < cap; i++)
            {
                AI tempObject = GameManager.instance.GetTeamMemberAt(GameManager.TeamEnum.Blue, i).GetComponent<AI>();
                if (tempObject != null)
                {
                    tempObject.SquadBehaviorActive(true);
                    tempObject.RallyPosition(transform.position);
                }
            }

            lastTime = Time.time + waitTime;
            aSource.PlayOneShot(aClip);
            m_WeaponController.SetState(WeaponController.WeaponState.Weapon);
        }
    }

    public void Hold()
    {
        if (device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
        {
            int cap = GameManager.instance.GetTeamCount(GameManager.TeamEnum.Blue);
            for (int i = 1; i < cap; i++)
            {
                AI tempObject = GameManager.instance.GetTeamMemberAt(GameManager.TeamEnum.Blue, i).GetComponent<AI>();
                if (tempObject != null)
                {
                    tempObject.SquadBehaviorActive(true);
                    tempObject.RallyPosition(tempObject.transform.position);
                }
            }

            lastTime = Time.time + waitTime;
            aSource.PlayOneShot(aClip);
            m_WeaponController.SetState(WeaponController.WeaponState.Weapon);
        }
    }

    public void Roam()
    {
        if (device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
        {
            int cap = GameManager.instance.GetTeamCount(GameManager.TeamEnum.Blue);
            for (int i = 1; i < cap; i++)
            {
                AI tempObject = GameManager.instance.GetTeamMemberAt(GameManager.TeamEnum.Blue, i).GetComponent<AI>();
                if (tempObject != null)
                {
                    tempObject.SquadBehaviorActive(false);
                }
            }

            lastTime = Time.time + waitTime;
            aSource.PlayOneShot(aClip);
            m_WeaponController.SetState(WeaponController.WeaponState.Weapon);
        }
    }
}
