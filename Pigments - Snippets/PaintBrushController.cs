using UnityEngine;
using UnityEngine.UI;

public class PaintBrushController : MonoBehaviour {
    [Header("Assigned Objects")]
    public GameObject viveCntrl;

    private Vector3 startPosition = Vector3.zero;
    private Vector3 endPosition = Vector3.zero;

    [Header("Powers prefabs")]
    public GameObject shield;
    public GameObject pod;
    public GameObject healing;

    [Header("Swipe indicator")]
    public Text indicator;

    private TrailRenderer trail;

    [Tooltip("Mana manager")]
    private ResourceManager resources;

    [Tooltip("Control toggler")]
    private ToggleControls m_Toggle;

    //Getting information from Vive Controller
    private SteamVR_TrackedObject trackedObject;
    private SteamVR_Controller.Device device { get { return SteamVR_Controller.Input((int)trackedObject.index); } }

    // Use this for initialization
    void Start()
    {
        trackedObject = viveCntrl.GetComponent<SteamVR_TrackedObject>();
        m_Toggle = GetComponentInParent<ToggleControls>();

        //De-activate powers on Startup
        shield.gameObject.SetActive(false);
        pod.gameObject.SetActive(false);

        trail = gameObject.GetComponentInChildren<TrailRenderer>();
        trail.enabled = false;

        resources = GetComponentInParent<ResourceManager>();

        if (healing == null) {
            healing = GameObject.Find("Healing");
        }
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (m_Toggle.GetState() == ToggleControls.State.Swiping)
        {
            if (device.GetPressDown(SteamVR_Controller.ButtonMask.Trigger))
            {
                startPosition = Camera.main.WorldToScreenPoint(gameObject.transform.position);
                //startPosition = gameObject.transform.localPosition;
                //Debug.Log(startPosition);
                trail.enabled = true;
            }

            if (device.GetPressUp(SteamVR_Controller.ButtonMask.Trigger))
            {
                endPosition = Camera.main.WorldToScreenPoint(gameObject.transform.position);
                //endPosition = gameObject.transform.localPosition;
                //Debug.Log(endPosition);

                trail.enabled = false;

                DirectionalVector(startPosition.x, endPosition.x, startPosition.y, endPosition.y);
            }
        }
    }

    public void Disable()
    {
        pod.SetActive(false);
        shield.SetActive(false);
    }

    private void DebugSwipe()
    {
        float change = endPosition.x - startPosition.x;
        indicator.text = string.Format("SX:{0} , EX:{1}, Change:{2}", startPosition.x, endPosition.x, change);
    }

    private void DisableXRay() {
        GameManager.instance.EnableXRay(GameManager.TeamEnum.Red, false);
    }

    private void DirectionalVector(float _startX, float _endX, float _startY, float _endY)
    {
        string type;

        float changeInX = endPosition.x - startPosition.x;
        float changeInY = endPosition.y - startPosition.y;

        Vector2 tempSwipe = new Vector3(changeInX, changeInY);
        tempSwipe.Normalize();

        //swipe upwards
        if (tempSwipe.y > 0 && tempSwipe.x > -0.5f && tempSwipe.x < 0.5f)
        {
            if (healing.activeSelf) {
                return;
            }

            if (resources.TakeResouces(40))
            {
                type = "Up Swipe";
                indicator.text = type;
                m_Toggle.SetState(ToggleControls.State.Hand);

                healing.SetActive(true);
            }
            else
            {
                type = "No Mana";
                indicator.text = type;
            }
        }

        //swipe down
        if (tempSwipe.y < 0 && tempSwipe.x > -0.5f && tempSwipe.x < 0.5f)
        {
            if (pod.gameObject.activeSelf) {
                return;
            }

            if (resources.TakeResouces(25))
            {
                type = "Down Swipe";
                indicator.text = type;
                m_Toggle.SetState(ToggleControls.State.Grasping);

                shield.gameObject.SetActive(false);
                pod.gameObject.SetActive(true);
            }
            else
            {
                type = "No Mana";
                indicator.text = type;
            }
        }

        //swipe left
        if (tempSwipe.x < 0 && tempSwipe.y > -0.5f && tempSwipe.y < 0.5f)
        {
            //if (grenade.gameObject.activeSelf) {
            //    return;
            //}

            m_Toggle.m_ViveController.SetActive(false);
            if (resources.TakeResouces(65))
            {
                type = "Left Swipe";
                indicator.text = type;
                m_Toggle.SetState(ToggleControls.State.Hand);
                GameManager.instance.EnableXRay(GameManager.TeamEnum.Red, true);
                Invoke("DisableXRay", 10.0f);
                shield.gameObject.SetActive(false);
                pod.gameObject.SetActive(false);
            }
            else
            {
                type = "No Mana";
                indicator.text = type;
            }
        }

        //swipe right
        if (tempSwipe.x > 0 && tempSwipe.y > -0.5f && tempSwipe.y < 0.5f)
        {
            if (shield.gameObject.activeSelf){
                return;
            }

            if (resources.TakeResouces(60))
            {
                type = "Right Swipe";
                indicator.text = type;
                m_Toggle.SetState(ToggleControls.State.Grasping);

                shield.gameObject.SetActive(true);
                pod.gameObject.SetActive(false);
            }
            else
            {
                type = "No Mana";
                indicator.text = type;
            }
        }
    }
}
