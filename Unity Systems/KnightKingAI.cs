
using UnityEngine;
using UnityEngine.AI;

public class KnightKingAI : Entity {

    FSM stateMachine;

    private FSM.FSMState introState;
    private FSM.FSMState wanderState;
    private FSM.FSMState chaseState;
    private FSM.FSMState attackState;
    private FSM.FSMState evolveState;

    float previousTime;

    private bool hasTarget;
    Transform target;

    Entity entity;

    NavMeshAgent pathfinder;

    float attackDistanceThreshold = .5f;

    float barrageRate = 0;

    float nextAttackTime = 0;
    float myCollisionRadius;
    float targetCollisionRadius;

    int currentLimit;

    [System.Serializable]
    public class Personalities
    {
        public string name;
        [Range(0, 5)]
        public float atkRate;
        [Range(0, 10)]
        public float speed;
        [Range(0, 4)]
        public int state;
        [Range(0, 2)]
        public int limit;
    }

    public Personalities persona;

    public float speedSmoothTime = 0.1f;
    MeleeWeapon[] meleeWeapon;

    InRange inRange;
    float refreshRateInSeconds;

    public GameObject shield;

    float threshold;

    NemesisTrigger nemesisTrigger;

    // Use this for initialization
    protected override void Start()
    {
        base.Start();

        stateMachine = new FSM();

        // Call Actions
        Intro();
        Wander();
        Chase();
        Attack();
        Evolve();

        stateMachine.Push(introState);

        previousTime = 0;

        refreshRateInSeconds = 1f;

        currentLimit = 0;
        threshold = .75f;

        hurtBox = GetComponent<CapsuleCollider>();
        pathfinder = GetComponent<NavMeshAgent>();
        meleeWeapon = GetComponentsInChildren<MeleeWeapon>();
        inRange = GetComponentInChildren<InRange>();
        nemesisTrigger = GetComponent<NemesisTrigger>();

        pathfinder.speed = persona.speed;

        GetTarget();

        shield.SetActive(false);

        if (target != null)
            transform.LookAt(target);

        AIManager.instance.guiManager.SetBossEntity(this.GetComponent<Entity>());
    }

    // Update is called once per frame
    protected override void FixedUpdate()
    {
        stateMachine.Update(gameObject);
        animator.SetFloat(Utility.spdPercent, pathfinder.velocity.magnitude, speedSmoothTime, Time.deltaTime);

        if (persona.limit > currentLimit && m_Health < (baseHealth * threshold)) {
            threshold -= .35f;
            stateMachine.Push(evolveState);
        }

        if (currentLimit == 2)
        {
            if (Time.time > barrageRate)
            {
                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.pBlastParticle), AIManager.instance.spawnPoints[Random.Range(0, AIManager.instance.spawnPoints.Length)].transform.position, transform.rotation, Vector3.one);
                barrageRate = Time.time + 1f;
            }
        }
      
        if (mShield > 0) {
            shield.SetActive(true);
            guardState = GuardState.Shield;
        } else {
            shield.SetActive(false);
            guardState = GuardState.Open;
        }
    }

    public void Intro()
    {
        introState = (FSM, gameObj) =>
        {
            nemesisTrigger.Scene();
            stateMachine.Push(wanderState);
        };
    }

    public void Wander()
    {
        wanderState = (fsm, gameObj) =>
        {
            pathfinder.Warp(transform.position);

            if (target != null)
                stateMachine.Push(chaseState);
            else if (inRange.GetRange() && Time.time > nextAttackTime)
                stateMachine.Push(attackState);
        };
    }

    public void Chase()
    {
        chaseState = (fsm, gameObj) =>
        {
            if (hasTarget)
            {
                if (target == null)
                    stateMachine.Pop();

                if (currentLimit == 1) {
                    shield.SetActive(true);
                }

                if (Time.time > (refreshRateInSeconds + previousTime))
                {
                    Vector3 dirToTarget = (target.position - transform.position).normalized;
                    Vector3 targetPosition = target.position - dirToTarget * (myCollisionRadius + targetCollisionRadius + attackDistanceThreshold * 0.5f);

                    if (!isDead)
                    {
                        pathfinder.SetDestination(targetPosition);
                        previousTime = Time.time;
                    }
                }

                //Enemy is in Range
                if (inRange.GetRange() && Time.time > nextAttackTime)    
                    stateMachine.Push(attackState);
            } else {
                stateMachine.Pop();
            }
        };
    }

    public void Evolve()
    {
        evolveState = (fsm, gameObj) =>
        {
            currentLimit++;
            if (currentLimit == 1)
                mShield += 5;
            else if (currentLimit == 2)
                barrageRate = Time.time + 0.75f;

            nemesisTrigger.Banter();
            stateMachine.Pop();
        };
    }

    public void Attack()
    {
        attackState = (fsm, gameObj) =>
        {
            if (!isDead)
            {
                transform.LookAt(target);

                AttactState();
                nextAttackTime = Time.time + persona.atkRate;
            }

            stateMachine.Pop();
        };
    }

    private void AttactState()
    {
        int rand = Random.Range(0, 2);

        if (rand == 0) {
            meleeWeapon[Random.Range(0, meleeWeapon.Length)].ActivateHitBox(animator);
        } else {
            animator.SetTrigger(Utility.stmpAtkStr);
            TakeMana(50);
        }
    }

    void OnTargetDeath()
    {
        hasTarget = false;
    }

    private void GetTarget()
    {
        hasTarget = true;
        target = AIManager.instance.GetTarget();

        if (target != null)
        {
            entity = target.GetComponent<Entity>();
            entity.OnDeath += OnTargetDeath;

            myCollisionRadius = hurtBox.radius;
            targetCollisionRadius = target.GetComponent<CapsuleCollider>().radius;
        }
    }

    public override void TakeDamage(float dmg, bool unblockable, Entity _entity)
    {
        base.TakeDamage(dmg, unblockable, _entity);
    }

    public override void OnObjectReuse()
    {
        gameObject.SetActive(true);
        if (stateMachine != null)
        {
            m_Health = baseHealth;
            mEndurance = baseEndurance;
            guardState = GuardState.Open;
            isDead = false;
            hurtBox.enabled = true;
            inRange.SetRange(false);

            //StartRefill();

            gameObject.layer = 10;

            stateMachine.Clear();
            stateMachine.Push(wanderState);
        }
    }
}
