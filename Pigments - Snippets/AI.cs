using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class AI : MonoBehaviour
{
    //Consts
    private const float ANIM_THRES = 0.1f;
    private const float TARGET_LOCK_MIN = 2.5f;
    private const float TARGET_LOCK_MAX = 5f;
    private const float GUN_TIMER_MED = 0.45f;
    private const float EVALUATE_MAX = 0.04f;
    private const float STATE_POP = 0.05f;
    public const float orbitRadius = 2.5f;
    private const float orbitRadiusSquared = orbitRadius * orbitRadius;
    private const float BURST_FIRE_RATE = 0.12f;
    private const float KILL_TIME = 3.0f;
    private const float SHOOT_TIME = 0.5f;
    private const int PLAYER_LAYER = 11;
    private const int ENEMY_LAYER = 16;
    private const int HAND_LAYER = 9;
    private const int COVER_LAYER = 14;
    private const float ROT_SPEED = 10;

    //private
    private float pathTime;
    private Vector3 RallyPos;
    private Vector3 PosMem;
    private bool squadBehaviorActive;
    private bool rallyBool;
    private float animTime;
    public enum States { Attack, Patrol, Orbit, Alerted, DM, Rally };
    private States currentState;
    private Queue<States> stateQue = new Queue<States>();
    private UnityEngine.AI.NavMeshAgent m_NavAgent;
    private Vector3 storedPos;
    private Camera m_Camera;
    private PoolSystem.Pool m_PaintballPool;
    [SerializeField]
    private Transform target;
    private HealthManager m_HealthManager;
    private bool isShooting;
    private float targetLockTimer;
    private float gunTimer;
    private float cosTurn;
    private float sinTurn;
    private bool attackBool;
    private bool patrolBool;
    private bool orbitBool;
    private bool alertedBool;
    private bool defensiveMovementBool;
    private bool loaded;
    private bool nameReady;
    [SerializeField]
    private List<GameObject> visibleObjects;
    private string AIname;
    private Transform textTransform;
    private TextMesh nameTextMesh;
    private Transform m_HealthBarTransform;
    private TextMesh m_HealthTextMesh;
    private Animator anim;
    private GameObject rifle;
    private Collider nameCheckPoint;
    private Vector3 rotationDir;

    //public
    public GameObject gunfire;
    public LayerMask layerNames;
    public LayerMask layerNorm;
    [Header("Debug")]
    public bool editorDebugMode = false;
    [Header("Leader")]
    public bool isLeader;
    [Header("Behaviors")]
    public float maxFleeDistance = 200.0f;
    public float maxPatrolDistance = 200.0f;
    public float maxAttackDistance = 25.0f;
    [Header("Gun")]
    //public GameObject paintball;
    public float m_FireRate = 350.0f;
    public Transform m_Gun;
    public float m_Force = 3500.0f;
    public SkinnedMeshRenderer m_SkinRenderer;

    [Header("Sounds")]
    public AudioClip[] fireClips;

    private AudioSource aSource;
    //public bool paused = false;
    // Use this for initialization
    void Start()
    {
        StartCoroutine("Setup");
    }

    void Init()
    {
        pathTime = animTime = 0;
        rotationDir = RallyPos = PosMem = storedPos = Vector3.zero;
        nameCheckPoint = GameObject.Find("NameCheck").GetComponent<Collider>();
        rifle = GetComponentInChildren<FindRifle>().gameObject;
        anim = GetComponentInChildren<Animator>();
        aSource = GetComponent<AudioSource>();
        visibleObjects = new List<GameObject>();
        nameReady = false;
        //Get Components
        m_SkinRenderer = GetComponentInChildren<SkinnedMeshRenderer>();
        m_HealthManager = GetComponentInChildren<HealthManager>();
        m_NavAgent = GetComponent<UnityEngine.AI.NavMeshAgent>();
        m_NavAgent.updateRotation = false;
        m_Camera = GetComponentInChildren<Camera>();
        m_PaintballPool = PoolSystem.FindPool("Projectile");
        //set the health according to what enemy type it is
        if (isLeader)
        {
            m_HealthManager.SetEntityToMinionLeader();
            transform.localScale = new Vector3(0.75f, 0.75f, 0.75f);
        }
        else m_HealthManager.SetEntityToMinion();
        currentState = States.Patrol;

        //set things to false
        SwitchOff();

        //set things to zero 
        gunTimer = targetLockTimer = pathTime = 0; //floats

        target = null;

        //precalculations
        m_FireRate = 60.0f / m_FireRate;
        sinTurn = Mathf.Sin(30.0f);
        cosTurn = Mathf.Cos(30.0f);

    }

    IEnumerator Setup()
    {
        Init();
        yield return new WaitUntil(NameReady);
        loaded = true;
        ChangeTextBox();
    }

    // Update is called once per frame

    void FixedUpdate()
    {
        if (loaded)
        {
            if (gunTimer > 0 && isShooting)
            {
                Instantiate(gunfire, m_Gun, false);
            }
            if (target == null)
            {
                rotationDir = (storedPos - transform.position).normalized;
                Quaternion quat = Quaternion.LookRotation(rotationDir, Vector3.up);
                quat.x = 0;
                quat.z = 0;
                transform.rotation = Quaternion.Slerp(transform.rotation, quat, Time.deltaTime * ROT_SPEED);
            }
            else
            {
                rotationDir = (target.position - transform.position).normalized;
                Quaternion quat = Quaternion.LookRotation(rotationDir, Vector3.up);
                quat.x = 0;
                quat.z = 0;
                transform.rotation = Quaternion.Slerp(transform.rotation, quat, Time.deltaTime * ROT_SPEED);
            }

            if (animTime <= 0)
            {
                if (PosMem == transform.position || pathTime <= 0 || !m_NavAgent.isActiveAndEnabled)
                    Walking(false);
                else
                    Walking(true);
                PosMem = transform.position;
                animTime = ANIM_THRES;
            }
            if (target != null && target.gameObject.layer == ENEMY_LAYER && !target.GetComponent<UnityEngine.AI.NavMeshAgent>().isActiveAndEnabled)
            {
                TargetRemoval();
            }
            RaycastHit hitInfo;
            if (Physics.Raycast(transform.position, nameCheckPoint.transform.position - transform.position, out hitInfo, m_Camera.farClipPlane, layerNames) && hitInfo.collider == nameCheckPoint)
            {
                if (CompareTag("Blue"))
                {
                    nameTextMesh.color = Color.blue;
                }
                else
                {
                    nameTextMesh.color = Color.red;
                }

                textTransform.gameObject.SetActive(true);
                m_HealthBarTransform.gameObject.SetActive(true);
                textTransform.LookAt(Camera.main.transform.position);
                textTransform.forward = -textTransform.forward;
                Quaternion textRot = textTransform.rotation;
                textRot.x = 0;
                textRot.z = 0;
                textTransform.rotation = textRot;

                m_HealthBarTransform.LookAt(Camera.main.transform.position);
                m_HealthBarTransform.forward = -m_HealthBarTransform.forward;
                textRot = m_HealthBarTransform.rotation;
                textRot.x = textRot.z = 0;
                m_HealthBarTransform.rotation = textRot;
                int trunc = (int)((GetHealthRatio() * 100.0f) + 0.5f);

                m_HealthTextMesh.text = trunc.ToString() + "%";
                if (trunc >= 75)
                {
                    m_HealthTextMesh.color = Color.green;
                }
                else if (trunc >= 50 && trunc < 75)
                {
                    m_HealthTextMesh.color = Color.yellow;
                }
                else if (trunc >= 25 && trunc < 50)
                {
                    Color orange = new Color(1, 0.5f, 0, 1);
                    m_HealthTextMesh.color = orange;
                }
                else
                {
                    m_HealthTextMesh.color = Color.red;
                }
            }
            else
            {
                textTransform.gameObject.SetActive(false);
                m_HealthBarTransform.gameObject.SetActive(false);
            }
            TimerCheck();
            SwitchOff();
            //State Machine
            switch (currentState)
            {
                case States.Attack:
                    attackBool = true;
                    break;
                case States.Patrol:
                    patrolBool = true;
                    break;
                case States.Orbit:
                    orbitBool = true;
                    break;
                case States.Alerted:
                    alertedBool = true;
                    break;
                case States.DM:
                    defensiveMovementBool = true;
                    break;
                case States.Rally:
                    rallyBool = true;
                    break;
            }
            StatePop();
        }
    }

    void SwitchOff() { isShooting = attackBool = patrolBool = orbitBool = alertedBool = defensiveMovementBool = rallyBool = false; }

    public Transform GetTargetPosition() { return target; }

    void Attack()
    {
        RaycastHit rayInfo;
        if (target != null)
        {
            if ((Physics.Raycast(m_Gun.position, target.transform.position - m_Gun.position, out rayInfo, m_Camera.farClipPlane, layerNorm)))
            {
                if (rayInfo.distance > maxAttackDistance)
                    StandardEnqueue(States.Orbit);
                else if (rayInfo.collider.transform.parent != null && !(rayInfo.collider.transform.parent.CompareTag("Cover")))
                {
                    isShooting = true;
                }
            }
        }
    }

    void Orbit()
    {
        if (target != null && m_NavAgent.isActiveAndEnabled && pathTime <= 0)
        {
            Vector3 offsetFromLeader = target.transform.position - transform.position;
            Vector3 rotatedVector = (Random.Range(0, 2) == 0) ? new Vector3((offsetFromLeader.x * cosTurn) - (offsetFromLeader.z * sinTurn), 0, (offsetFromLeader.x * sinTurn) + (offsetFromLeader.z * cosTurn)) : new Vector3((offsetFromLeader.x * -cosTurn) - (offsetFromLeader.z * -sinTurn), 0, (offsetFromLeader.x * -sinTurn) + (offsetFromLeader.z * -cosTurn));
            storedPos = target.transform.position + rotatedVector;
            m_NavAgent.SetDestination(storedPos);
            pathTime = CalculateDistance(transform.position, storedPos) / m_NavAgent.speed;

        }
    }

    void DefensiveMovement()
    {
        if (target != null && m_NavAgent.isActiveAndEnabled && pathTime <= 0)
        {
            for (int i = 0; i < 100; ++i)
            {
                Vector3 randomDir = Random.insideUnitSphere * maxFleeDistance;
                randomDir.y = Random.Range(0, 3.0f);
                randomDir += transform.position;
                UnityEngine.AI.NavMeshHit hit;
                UnityEngine.AI.NavMesh.SamplePosition(randomDir, out hit, maxFleeDistance, UnityEngine.AI.NavMesh.AllAreas);

                if (hit.position.Equals(null)) { continue; }

                RaycastHit rayInfo;
                if (!(Physics.Raycast(hit.position, target.transform.position - hit.position, out rayInfo, maxFleeDistance, layerNorm))) { continue; }
                if (rayInfo.collider.transform.parent != null && rayInfo.collider.transform.parent.CompareTag("Cover"))
                {
                    storedPos = hit.position;
                    break;
                }
            }
            m_NavAgent.SetDestination(storedPos);
            pathTime = CalculateDistance(transform.position, storedPos) / m_NavAgent.speed;
        }
    }

    void Patrol()
    {
        if (target == null && m_NavAgent.isActiveAndEnabled && pathTime <= 0)
        {
            for (;;)
            {
                Vector3 randomDir = Random.insideUnitSphere * maxPatrolDistance;
                randomDir.y = Random.Range(0, 3.0f);
                randomDir += transform.position;
                UnityEngine.AI.NavMeshHit hit = new UnityEngine.AI.NavMeshHit();
                if (UnityEngine.AI.NavMesh.SamplePosition(randomDir, out hit, maxPatrolDistance, UnityEngine.AI.NavMesh.AllAreas))
                {
                    storedPos = hit.position;
                    m_NavAgent.SetDestination(storedPos);
                    pathTime = CalculateDistance(transform.position, storedPos) / m_NavAgent.speed;
                    break;
                }
            }
        }
    }

    public AudioSource GetAudioSource() { return aSource; }

    void Alerted()
    {
        if (target != null)
        {
            StandardEnqueue(States.Attack);

            if (!squadBehaviorActive)
            {
                if (GetHealth() <= (GetMaxHealth() * 0.15f) || (target.transform.position - transform.position).magnitude <= orbitRadiusSquared)
                    StandardEnqueue(States.DM);
                else
                    StandardEnqueue(States.Orbit);
            }
        }
    }

    void Look()
    {
        visibleObjects.Clear();
        Plane[] planes = GeometryUtility.CalculateFrustumPlanes(m_Camera);
        GameManager.TeamEnum _Team = (tag == GameManager.TeamEnum.Blue.ToString()) ? GameManager.TeamEnum.Blue : GameManager.TeamEnum.Red;
        GameManager.TeamEnum _EnemyTeam = (_Team == GameManager.TeamEnum.Blue) ? GameManager.TeamEnum.Red : GameManager.TeamEnum.Blue;
        int EnemyTeamCount = GameManager.instance.GetTeamCount(_EnemyTeam);
        GameObject tempEnemy = null;

        for (int i = 0; i < EnemyTeamCount; ++i)
        {
            tempEnemy = GameManager.instance.GetTeamMemberAt(_EnemyTeam, i);
            if (IsVisible(planes, tempEnemy))
            {
                RaycastHit hitInfo;
                if (!(Physics.Raycast(m_Camera.transform.position, tempEnemy.transform.position - m_Camera.transform.position, out hitInfo, m_Camera.farClipPlane, layerNorm)) || hitInfo.collider.gameObject.layer == COVER_LAYER)
                {
                    continue;
                }
                if (tempEnemy.layer == ENEMY_LAYER && hitInfo.collider.GetComponentInParent<UnityEngine.AI.NavMeshAgent>() == tempEnemy.GetComponent<UnityEngine.AI.NavMeshAgent>())
                    visibleObjects.Add(tempEnemy);
                else if (tempEnemy.layer == PLAYER_LAYER && tempEnemy.transform.position == hitInfo.collider.bounds.center)
                    visibleObjects.Add(tempEnemy);
                tempEnemy = null;
            }
        }

        //Random target, priority targets can be coded in later.
        if (visibleObjects.Count > 0 && targetLockTimer <= 0)
        {
            tempEnemy = visibleObjects[Random.Range(0, visibleObjects.Count)];
            target = tempEnemy.transform;
            targetLockTimer = Random.Range(TARGET_LOCK_MIN, TARGET_LOCK_MAX);
        }
        else
        {
            if (targetLockTimer <= 0)
            {
                target = null;
            }
        }

    }

    //Thinking
    void Evaluate()
    {
        Look();

        if (squadBehaviorActive)
        {
            StandardEnqueue(States.Rally);
        }

        if (target != null)
        {
            StandardEnqueue(States.Alerted);
        }
        else
        {
            //TargetRemoval();
            if (!squadBehaviorActive)
                StandardEnqueue(States.Patrol);
        }
    }

    //State Management
    void StatePop()
    {
        if (stateQue.Count > 0)
        {
            currentState = stateQue.Dequeue();
        }
    }

    void StandardEnqueue(States _state)
    {
        stateQue.Enqueue(_state);
    }

    bool IsVisible(Plane[] planes, GameObject obj)
    {
        if (obj.layer == ENEMY_LAYER)
            return (obj.activeInHierarchy) ? GeometryUtility.TestPlanesAABB(planes, obj.GetComponentInChildren<Collider>().bounds) : false;
        return (obj.activeInHierarchy) ? GeometryUtility.TestPlanesAABB(planes, obj.GetComponentInParent<CapsuleCollider>().bounds) : false;
    }

    void TimerCheck()
    {
        //TODO: Better implementation?
        if (targetLockTimer > 0)
        {
            targetLockTimer -= Time.deltaTime;
        }
        if (pathTime > 0)
        {
            pathTime -= Time.deltaTime;
        }
        if (animTime > 0)
        {
            animTime -= Time.deltaTime;
        }
    }

    public void TakeDamage(Transform _Target)
    {
        if (target != null && target.gameObject.layer == PLAYER_LAYER)
            return;
        if(target != _Target)
        {
            target = _Target;
            targetLockTimer = TARGET_LOCK_MAX;
        }
    }

    public float GetHealth() { return m_HealthManager.GetHealth(); }
    public float GetHealthRatio() { return m_HealthManager.GetHealthRatio(); }

    public float GetMaxHealth() { return m_HealthManager.GetMaxHealth(); }

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

    private IEnumerator Burst(float _killTime)
    {
        for (int i = 0; i < 3; i++)
        {
            if (m_PaintballPool == null) m_PaintballPool = PoolSystem.FindPool("Projectile");
            GameObject burst = m_PaintballPool.m_NextObject;
            burst.transform.position = m_Gun.position;
            burst.transform.rotation = m_Gun.rotation;
            burst.SetActive(true);

            float accuracy = 2.5f;
            CalculateAccuracy(ref burst, accuracy);

            int randomSound = Random.Range(0, fireClips.Length);
            aSource.PlayOneShot(fireClips[randomSound]);

            burst.GetComponent<Rigidbody>().AddForce(burst.transform.forward * m_Force);
            burst.GetComponent<Projectile>().SetOwner(gameObject, (CompareTag(GameManager.TeamEnum.Blue.ToString())) ? GameManager.TeamEnum.Blue : GameManager.TeamEnum.Red);

            yield return new WaitForSeconds(BURST_FIRE_RATE);
        }
        yield return null;
    }

    public void Fire()
    {
        anim.SetBool("Attack", true);
        anim.SetBool("Walk", false);
        rifle.transform.LookAt(target.position);
        int randomSound;
        GameObject obj;
        Transform objTransform;
        if (isLeader)
        {
            StartCoroutine("Burst", KILL_TIME);
        }
        else
        {
            if (m_PaintballPool == null) m_PaintballPool = PoolSystem.FindPool("Projectile");
            obj = m_PaintballPool.m_NextObject;
            if (obj == null) return;
            objTransform = obj.transform;
            objTransform.position = m_Gun.position;
            objTransform.rotation = m_Gun.rotation;
            obj.SetActive(true);

            randomSound = Random.Range(0, fireClips.Length);

            if (fireClips[randomSound] != null)
            {
                aSource.PlayOneShot(fireClips[randomSound]);
            }

            obj.GetComponent<Rigidbody>().AddForce(m_Gun.forward * m_Force);
            obj.GetComponent<Projectile>().SetOwner(gameObject, (CompareTag(GameManager.TeamEnum.Blue.ToString())) ? GameManager.TeamEnum.Blue : GameManager.TeamEnum.Red);
        }

    }

    IEnumerator ShootRout()
    {
        for (;;)
        {
            if (attackBool) Attack();

            if (isShooting)
            {
                Fire();
                gunTimer = m_FireRate * Random.Range(0.75f, 1.3f);
            }

            yield return new WaitForSeconds(gunTimer);
        }
    }
    IEnumerator EvalRout()
    {
        for (;;)
        {
            Evaluate();
            yield return new WaitForSeconds(EVALUATE_MAX);
        }
    }
    IEnumerator AlertedRout()
    {
        for (;;)
        {
            yield return new WaitUntil(AlertedBFunc);
            Alerted();
        }
    }
    bool AlertedBFunc() { return alertedBool; }

    IEnumerator OrbitRout()
    {
        for (;;)
        {
            yield return new WaitUntil(OrbitBFunc);
            Orbit();
        }
    }

    bool OrbitBFunc() { return orbitBool; }

    IEnumerator PatrolRout()
    {
        for (;;)
        {
            yield return new WaitUntil(PatrolBFunc);
            Patrol();
        }
    }

    bool PatrolBFunc() { return patrolBool; }

    bool DMBFunc() { return defensiveMovementBool; }

    IEnumerator DMRout()
    {
        for (;;)
        {
            yield return new WaitUntil(DMBFunc);
            DefensiveMovement();
        }
    }

    bool RallyBFunc() { return rallyBool; }

    IEnumerator RallyRout()
    {
        for (;;)
        {
            yield return new WaitUntil(RallyBFunc);
            if (squadBehaviorActive)
            {
                Rally();
            }
        }
    }

    private void Rally()
    {
        if (NavPosReached() || pathTime <= 0)
        {
            m_NavAgent.SetDestination(RallyPos);
            pathTime = CalculateDistance(transform.position, RallyPos) / m_NavAgent.speed;
        }
    }

    private bool NavPosReached()
    {
        if (m_NavAgent.pathEndPosition == RallyPos)
            return true;
        return false;
    }

    public void RallyPosition(Vector3 _pos) { RallyPos = _pos; }

    public void SquadBehaviorActive(bool _pred)
    {
        if (_pred)
        {
            m_NavAgent.ResetPath();
            stateQue.Clear();
        }
        squadBehaviorActive = _pred;
    }

    private void OnDrawGizmosSelected()
    {
        if (editorDebugMode)
        {
            Gizmos.color = Color.green;
            if (target != null)
                Gizmos.DrawWireSphere(target.position, 1.0f);
        }
    }

    private void TargetRemoval()
    {
        target = null;
        targetLockTimer = 0;
    }

    public string GetName()
    {
        return AIname;
    }
    public void SetName(string _name)
    {
        gameObject.name = _name;
        AIname = _name;
    }
    void ChangeTextBox()
    {
        TextMesh[] texts = GetComponentsInChildren<TextMesh>();

        for (int i = 0; i < texts.Length; ++i)
        {
            if (texts[i].name == "Minion Name")
            {
                textTransform = texts[i].transform;
                texts[i].text = AIname;
                nameTextMesh = texts[i];
            }
            else if (texts[i].name == "Minion Health")
            {
                m_HealthBarTransform = texts[i].transform;
                m_HealthTextMesh = texts[i];
            }
        }
    }
    private bool NameReady()
    {
        return nameReady;
    }
    public void SetNameReady(bool _pred)
    {
        nameReady = _pred;
    }
    public void Pause(bool _pred)
    {
        if (_pred)
        {
            TargetRemoval();
            StopAllCoroutines();
            stateQue.Clear();
            SwitchOff();
            visibleObjects.Clear();
            gunTimer = pathTime = animTime = targetLockTimer = 0;
        }
        else
        {
            StartCoroutine("RallyRout");
            StartCoroutine("ShootRout");
            StartCoroutine("EvalRout");
            StartCoroutine("AlertedRout");
            StartCoroutine("DMRout");
            StartCoroutine("PatrolRout");
            StartCoroutine("OrbitRout");
        }
    }
    public void Regenerate()
    {
        if (isLeader)
        {
            m_HealthManager.SetInvulnerable(true);
            Pause(true);
            Invoke("Regen", 5.0f);
        }
        StartCoroutine("Regen", 5);
    }
    void Regen()
    {
        Pause(false);
        m_HealthManager.SetInvulnerable(false);
        m_HealthManager.SetFullHealth();
        //GameManager.instance.ReviveLeader(GameManager.TeamEnum.Red);
    }
    public SkinnedMeshRenderer GetSMR() { return m_SkinRenderer; }
    public UnityEngine.AI.NavMeshAgent GetNavAgent() { return m_NavAgent; }

    public void SetAsLeader() { isLeader = true; }

    float CalculateDistance(Vector3 start, Vector3 des)
    {
        return Mathf.Sqrt(Mathf.Pow(des.x - start.x, 2) + Mathf.Pow(des.y - start.y, 2) + Mathf.Pow(des.z - start.z, 2));
    }
    void Walking(bool _pred)
    {
        if (_pred)
        {
            anim.SetBool("Walk", true);
            anim.SetBool("Attack", false);
        }
        else
        {
            anim.SetBool("Walk", false);
            anim.SetBool("Attack", false);
        }
    }
}
