// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,21.11.2019</created>
/// <changed>ʆϒʅ,22.11.2019</changed>
// ********************************************************************************

#ifndef POINTER_H
#define POINTER_H


////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CPointerComponent final : public IEntityComponent
{
public:
	virtual ~CPointerComponent() {}

	// IEntityComponent
	virtual void Initialize() override
	{
		// Set the model
		const int geometrySlot = 0;
		m_pEntity->LoadGeometry(geometrySlot, "Objects/aimingPointer.cgf");

		// Load the custom bullet material.
		// This material has the 'mat_bullet' surface type applied, which is set up to play sounds on collision with 'mat_default' objects in Libs/MaterialEffects
		auto *pBulletMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("Materials/aim");
		m_pEntity->SetMaterial(pBulletMaterial);

		// Now create the physical representation of the entity
		SEntityPhysicalizeParams physParams;
		physParams.type = PE_RIGID;
		physParams.mass = 0;
		m_pEntity->Physicalize(physParams);

		// Make sure that bullets are always rendered regardless of distance
		// Ratio is 0 - 255, 255 being 100% visibility
		GetEntity()->SetViewDistRatio(255);

		// Apply an impulse so that the bullet flies forward
		if (auto *pPhysics = GetEntity()->GetPhysics())
		{
			pe_action_impulse impulseAction;

			const float initialVelocity = 1000.f;

			// Set the actual impulse, in this cause the value of the initial velocity CVar in bullet's forward direction
			impulseAction.impulse = GetEntity()->GetWorldRotation().GetColumn1() * initialVelocity;

			// Send to the physical entity
			pPhysics->Action(&impulseAction);
		}
	}

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPointerComponent>& desc)
	{
		desc.SetGUID("{B53A9A5F-F27A-42CB-82C7-B1E379C41A2A}"_cry_guid);
	}

	virtual Cry::Entity::EventFlags GetEventMask() const override { return ENTITY_EVENT_COLLISION; }
	virtual void ProcessEvent(const SEntityEvent& event) override
	{
		// Handle the OnCollision event, in order to have the entity removed on collision
		if (event.event == ENTITY_EVENT_COLLISION)
		{
			// Collision info can be retrieved using the event pointer
			//EventPhysCollision *physCollision = reinterpret_cast<EventPhysCollision *>(event.ptr);

			// Queue removal of this entity, unless it has already been done
			gEnv->pEntitySystem->RemoveEntity(GetEntityId());
		}
	}
	// ~IEntityComponent
};


#endif // !POINTER_H
