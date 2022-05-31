class WarheadComponentClass : ScriptComponentClass
{
}

class WarheadComponent : ScriptComponent
{
	private static const int TRACE_ITERATIONS_PER_AXIS = 5;
	private static const int DECAL_COLOR = Color.FromRGBA(255, 255, 255, 255).PackToInt();
	private static const ref array<string> MATERIAL_PATHS =
	{
		"{7517F14CACE7D68A}materials/ExplosionScorchMark1.emat",
		"{096D7015B630FD15}materials/ExplosionScorchMark2.emat",
	};
	private static const ref array<ResourceName> PREFAB_PATHS =
	{
		"{DD8947C58DD25ABA}Prefabs/ScorchDecal1.et",
		"{B1DD73253116D594}Prefabs/ScorchDecal2.et",
	};

	private IEntity _owner;
	private World _world;
	private vector _origin;
	private bool _didAffectGrass;

	override void OnPostInit(IEntity owner)
	{
		_owner = owner;
		_world = owner.GetWorld();
		_origin = owner.GetOrigin();
		
		for (int i = 0; i < 5; ++i)
		{
			SpawnScorchMark(Vector(Math.RandomFloat(-1, 1), Math.RandomFloat(-1, 1), Math.RandomFloat(-1, 1)));
		}
	}

	private void SpawnScorchMark(vector direction)
	{
		auto surfaceTraceParam = GetSurfaceIntersection(direction);
		if (!surfaceTraceParam.TraceEnt || DecalEntity.Cast(surfaceTraceParam.TraceEnt))
		{
			return;
		}

		auto resource = Resource.Load(PREFAB_PATHS.GetRandomElement());
		if (!resource.IsValid())
		{
			Debug.Error("Resource failed to load");
			return;
		}

		// spawn scorch mark
		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;
		Math3D.AnglesToMatrix(direction.VectorToAngles(), spawnParams.Transform);
		SCR_Math3D.RotateAround(spawnParams.Transform, _origin, direction, Math.RandomFloat(0, 360), spawnParams.Transform);
		float scale = Math.RandomFloat(2, 5);
		spawnParams.Transform[0] = spawnParams.Transform[0] * scale;
		spawnParams.Transform[1] = spawnParams.Transform[1] * scale;
		spawnParams.Transform[2] = spawnParams.Transform[2] * scale;
		spawnParams.Transform[3] = _origin;
		IEntity scorchMark = GetGame().SpawnEntityPrefabLocal(resource, _world, spawnParams);


		#ifdef WORKBENCH
		Shape.CreateSphere(COLOR_YELLOW_A, ShapeFlags.TRANSP | ShapeFlags.NOOUTLINE, scorchMark.GetOrigin(), 0.1);
		Shape.CreateArrow(scorchMark.GetOrigin(), scorchMark.GetOrigin() + scorchMark.GetAngles().Normalized(), 0.2, COLOR_YELLOW, ShapeFlags.DEFAULT);
		#endif

		// flatten grass
		if (!_didAffectGrass)
		{
			auto terrain = GenericTerrainEntity.Cast(surfaceTraceParam.TraceEnt);
			if (terrain)
			{
				terrain.FlattenGrassSphere(_origin[0], _origin[2], 10, 0.1, 5, 0.25);
				terrain.FlattenGrassSphere(_origin[0], _origin[2], 5, 0.05, 0.1, 0.5);
				terrain.FlattenGrassSphere(_origin[0], _origin[2], 2, 0.01, 0.03, 1);
				_didAffectGrass = true;
			}
		}
	}

	private TraceParam GetSurfaceIntersection(vector direction)
	{
		auto param = new TraceParam();
  		param.Start = _origin;
  		param.End = _origin + direction;
  		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  		param.Exclude = _owner;
		_world.TraceMove(param, null);
		return param;
	}
}