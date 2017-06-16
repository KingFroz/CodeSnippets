using System.Collections;
using UnityEngine;
using UnityEngine.UI;

public class WeaponController : PartManager
{
    [Header ("Assigned Objects")]
    public GameObject viveCntrl;
    public GameObject Player;

    private StockAttributes m_Stock;
    private BarrelAttributes m_Barrel;

    private int m_Max;

    public enum Part { Barrel, Stock, C02 };
    private Part m_Part;

    //Button Canvas
    [SerializeField]
    private Canvas feedback;

    public Image BarrelImg;
    public Image StockImg;

    [SerializeField]
    private GameObject m_StockPosition;
    [SerializeField]
    private GameObject m_BarrelPosition;

    private const float moveToPosition = 0.04f;
    private bool resetParts;

    public enum WeaponState { Weapon, Custom, Command }
    private WeaponState m_StateMachine; 

    private bool isShooting;
    private int m_Ammunition;

    private float lastFireTime = 0.0f;
    public float debugRate;

    private const float BURSTRATE = 0.12f;

    [Header("Gun Audio")]
    public AudioClip[] paintballFire;
    public AudioClip shotgunFire;
    public AudioClip[] rifleFire;
    public AudioClip empty;

    private AudioSource aSource;

    private SquadCommand Commander;
    private PoolSystem.Pool m_Pool;
    private PoolSystem.Pool g_Pool;

    private bool reset; //In Charge of telling game to Re-Get Components

    //Getting information from Vive Controller
    private SteamVR_TrackedObject trackedObject;
    private SteamVR_Controller.Device device { get { return SteamVR_Controller.Input((int)trackedObject.index); } }

    // Use this for initialization
    void Start()
    {
        trackedObject = viveCntrl.GetComponent<SteamVR_TrackedObject>();
        m_Pool = PoolSystem.FindPool("Projectile");
        g_Pool = PoolSystem.FindPool("Grenade");

        m_Stock = GetComponentInChildren<StockAttributes>();
        m_Barrel = GetComponentInChildren<BarrelAttributes>();

        activeBarrelList.Add(m_BarrelParts[0]);
        activeStockList.Add(m_StockParts[0]);

        m_BarrelIndex = 0;
        m_StockIndex = 0;

        //manager = GetComponentInChildren<PartManager>();
        Commander = GetComponent<SquadCommand>();

        aSource = GetComponent<AudioSource>();

        isShooting = false;
        m_Ammunition = 200;

        m_Part = Part.Barrel;
        m_StateMachine = WeaponState.Weapon;

        reset = false;
        resetParts = true;

        feedback.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        isShooting = false;

        //Tell WeaponController to re-get children using Event
        if (reset) {
            m_Stock = GetComponentInChildren<StockAttributes>();
            m_Barrel = GetComponentInChildren<BarrelAttributes>();

            reset = false;
        }

        switch (m_StateMachine)
        {
            case WeaponState.Weapon:
                if (device.GetPressDown(SteamVR_Controller.ButtonMask.Touchpad)) {
                    m_StateMachine = WeaponState.Command;
                }

                if (device.GetPressUp(SteamVR_Controller.ButtonMask.ApplicationMenu)) {
                    m_StateMachine = WeaponState.Custom;
                    resetParts = true;
                }

                if (m_Ammunition <= 0 && device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
                {
                    if (!aSource.isPlaying) {
                        aSource.PlayOneShot(empty);
                    }
                }

                //Shooting
                if (m_Ammunition > 0 && device.GetPress(SteamVR_Controller.ButtonMask.Trigger))
                {
                    isShooting = true;
                    Fire();
                }
                break;
            case WeaponState.Custom:
                feedback.enabled = true;

                if (m_Part == Part.Barrel) {
                    BarrelImg.color = Color.red;
                    StockImg.color = Color.white;
                } else {
                    BarrelImg.color = Color.white;
                    StockImg.color = Color.red;
                }

                if (resetParts)
                {
                    m_BarrelPosition.transform.localPosition = new Vector3(0, 0, moveToPosition);
                    m_StockPosition.transform.localPosition = new Vector3(0, 0, -moveToPosition);

                    resetParts = false;
                }

                if (device.GetPress(SteamVR_Controller.ButtonMask.Trigger) || Input.GetKey(KeyCode.C)) {
                    m_BarrelPosition.transform.localPosition = new Vector3(0, 0, 0);
                    m_StockPosition.transform.localPosition = new Vector3(0, 0, 0);

                    feedback.enabled = false;
                    m_StateMachine = WeaponState.Weapon;
                }

                if (Input.GetButtonUp("Fire1") || device.GetPressUp(SteamVR_Controller.ButtonMask.ApplicationMenu))
                {
                    ToggleParts(m_Part);
                }

                if (Input.GetKeyUp(KeyCode.UpArrow) || device.GetAxis().y > 0 && device.GetAxis().x > -0.5f && device.GetAxis().x < 0.5f && device.GetPressDown(SteamVR_Controller.ButtonMask.Touchpad))
                {
                    //Up 
                    if (m_Part == Part.Barrel) {
                        m_Part = Part.Stock;
                    } else {
                        m_Part = Part.Barrel;
                    }
                }
                else if (Input.GetKeyUp(KeyCode.DownArrow) || device.GetAxis().y < 0 && device.GetAxis().x > -0.5f && device.GetAxis().x < 0.5f && device.GetPressDown(SteamVR_Controller.ButtonMask.Touchpad))
                {
                    //Down
                    if (m_Part == Part.Barrel) {
                        m_Part = Part.Stock;
                    } else {
                        m_Part = Part.Barrel;
                    }
                }

                break;
            case WeaponState.Command:
                switch (Commander.GetOrder())
                {
                    case SquadCommand.Order.Regroup:
                        Commander.ReGroup();
                        break;
                    case SquadCommand.Order.Hold:
                        Commander.Hold();
                        break;
                    case SquadCommand.Order.Roam:
                        Commander.Roam();
                        break;
                    case SquadCommand.Order.Move:
                        Commander.MoveToPosition();
                        break;
                    case SquadCommand.Order.NOTOUCH:
                        m_StateMachine = WeaponState.Weapon;
                        break;
                }

                break;
        }
    }

    public Transform GetMuzzlePosition() {
        return m_Barrel.GetMuzzle();
    }

    public void SetState(WeaponState _state) {
        m_StateMachine = _state;
    }

    public WeaponState GetState() {
        return m_StateMachine;
    }

    private void ToggleParts(Part _part)
    {
        int tempIndex;
        switch (_part)
        {
            case Part.Barrel:
                //tempIndex = manager.GetBarrelIndex();
                tempIndex = GetBarrelIndex();
                m_Max = GetNumBarrelParts();

                tempIndex++;

                if (tempIndex >= m_Max) {
                    tempIndex = 0;
                } else if (tempIndex < 0) {
                    tempIndex = (m_Max - 1);
                }

                SetIndex("Barrel", tempIndex);
                Reset();
                break;
            case Part.Stock:
                tempIndex = GetStockIndex();
                m_Max = GetNumStockParts();

                tempIndex++;

                if (tempIndex >= m_Max) {
                    tempIndex = 0;
                } else if (tempIndex < 0) {
                    tempIndex = (m_Max - 1);
                }

                SetIndex("Stock", tempIndex);
                Reset();
                break;
            case Part.C02:
                break;
        }
    }

    private void SingleFire(AudioClip[] _clip, float _killTime)
    {
        Projectile getProjectile;
        GameObject projectile = m_Pool.m_NextObject;
        projectile.transform.position = m_Barrel.GetMuzzle().position;
        projectile.transform.rotation = m_Barrel.GetMuzzle().rotation;
        projectile.SetActive(true);

        CalculateAccuracy(ref projectile, m_Stock.GetAccuracy());

        int randomSound = Random.Range(0, _clip.Length);
        aSource.PlayOneShot(_clip[randomSound]);

        projectile.GetComponent<Rigidbody>().AddForce(projectile.transform.forward * m_Barrel.GetSpeed());

        getProjectile = projectile.GetComponent<Projectile>();
        getProjectile.SetOwner(Player, GameManager.TeamEnum.Blue, true);
        getProjectile.SetKillTime(_killTime);

        --m_Ammunition;
        device.TriggerHapticPulse(2500);
    }

    private void LauncherFire(AudioClip[] _clip, float _killTime)
    {
        Projectile getProjectile;
        GameObject projectile = g_Pool.m_NextObject;
        projectile.transform.position = m_Barrel.GetMuzzle().position;
        projectile.transform.rotation = m_Barrel.GetMuzzle().rotation;
        projectile.SetActive(true);

        CalculateAccuracy(ref projectile, m_Stock.GetAccuracy());

        int randomSound = Random.Range(0, _clip.Length);
        aSource.PlayOneShot(_clip[randomSound]);

        projectile.GetComponent<Rigidbody>().AddForce(projectile.transform.forward * m_Barrel.GetSpeed());

        getProjectile = projectile.GetComponent<Projectile>();
        getProjectile.SetOwner(Player, GameManager.TeamEnum.Blue, true);
        getProjectile.SetKillTime(_killTime);

        --m_Ammunition;
        device.TriggerHapticPulse(2500);
    }

    private void ShotgunFire(int _max, float _killTime)
    {
        Projectile getProjectile;
        aSource.PlayOneShot(shotgunFire);
        for (int i = 0; i < _max; ++i)
        {
            if (m_Ammunition <= 0) { return; }
            GameObject projectile = m_Pool.m_NextObject;
            projectile.transform.position = m_Barrel.GetMuzzle().position;
            projectile.transform.rotation = m_Barrel.GetMuzzle().rotation;
            projectile.SetActive(true);

            CalculateAccuracy(ref projectile, m_Stock.GetAccuracy());

            projectile.GetComponent<Rigidbody>().AddForce(projectile.transform.forward * m_Barrel.GetSpeed());
            getProjectile = projectile.GetComponent<Projectile>();
            getProjectile.SetOwner(Player, GameManager.TeamEnum.Blue, true);
            getProjectile.SetKillTime(_killTime);

            --m_Ammunition;
            device.TriggerHapticPulse(2500);
        }
    }

    private void WeaopnHandle()
    {
        if (!isShooting) return;

        switch (m_Barrel.GetFireType())
        {
            case BarrelAttributes.FireType.Single:
                SingleFire(paintballFire, 2.5f);
                break;
            case BarrelAttributes.FireType.ShotGun:
                ShotgunFire(5, 0.25f);
                break;
            case BarrelAttributes.FireType.Burst:
                StartCoroutine(Burst(paintballFire, 2.5f));
                break;
            case BarrelAttributes.FireType.Rifle:
                SingleFire(rifleFire, 5.0f);
                break;
            case BarrelAttributes.FireType.Launcher:
                LauncherFire(paintballFire, 3.5f);
                break;
        }
    }
    
    public void Reset() {
        reset = true;
    }

    public int getAmmoCount() {
        return m_Ammunition <= 0 ? 0 : m_Ammunition;
    }

    public void refill(int amount) {
        m_Ammunition += amount;
    }

    void CalculateAccuracy(ref GameObject _rotate, float _accuracy)
    {
        float randX = 0;
        float randY = 0;
        float randZ = 0;

        randX = Random.Range(-_accuracy, _accuracy);
        randY = Random.Range(-_accuracy, _accuracy);
        randZ = Random.Range(-_accuracy, _accuracy);

        _rotate.transform.Rotate(randX, randY, randZ);
    }

    private void Fire()
    {
        if (Time.time > (m_Barrel.GetFireRate()) + lastFireTime)
        {
            WeaopnHandle();
            device.TriggerHapticPulse(2500);
            lastFireTime = Time.time;
        }
    }

    private IEnumerator Burst(AudioClip[] _clip, float _killTime)
    {
        for (int i = 0; i < 3; i++)
        {
            if (getAmmoCount() <= 0) { yield return null; }

            GameObject burst = m_Pool.m_NextObject;
            burst.transform.position = m_Barrel.GetMuzzle().position;
            burst.transform.rotation = m_Barrel.GetMuzzle().rotation;
            burst.SetActive(true);

            CalculateAccuracy(ref burst, m_Stock.GetAccuracy());

            int randomSound = Random.Range(0, _clip.Length);
            aSource.PlayOneShot(_clip[randomSound]);

            burst.GetComponent<Rigidbody>().AddForce(burst.transform.forward * m_Barrel.GetSpeed());

            Projectile getProjectile;
            getProjectile = burst.GetComponent<Projectile>();
            getProjectile.SetOwner(Player, GameManager.TeamEnum.Blue, true);
            getProjectile.SetKillTime(_killTime);

            --m_Ammunition;
            device.TriggerHapticPulse(2500);

            yield return new WaitForSeconds(BURSTRATE);
        }

        yield return null;
    }
}
