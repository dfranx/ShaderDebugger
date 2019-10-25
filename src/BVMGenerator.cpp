#include <ShaderDebugger/BVMGenerator.h>

#include <aGen/aGen.hpp>

// Glslang includes
#include <glslang/MachineIndependent/SymbolTable.h>
#include <glslang/Include/Common.h>
#include <glslang/Include/revision.h>

#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace sd {
	class TGlslangToBVMTraverser : public glslang::TIntermTraverser {
	public:
		TGlslangToBVMTraverser(const glslang::TIntermediate* inter) : m_glslang(inter) {}
		virtual ~TGlslangToBVMTraverser() { }

		void init() { m_gen.SetHeader(1, 3); m_isHLSL = false; }

		void visitSymbol(glslang::TIntermSymbol* symbol);
		bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate*) {}
		bool visitBinary(glslang::TVisit, glslang::TIntermBinary*) {}
		void visitConstantUnion(glslang::TIntermConstantUnion*) {}
		bool visitSelection(glslang::TVisit, glslang::TIntermSelection*) {}
		bool visitSwitch(glslang::TVisit, glslang::TIntermSwitch*) {}
		bool visitUnary(glslang::TVisit, glslang::TIntermUnary*) {}
		bool visitLoop(glslang::TVisit, glslang::TIntermLoop*) {}
		bool visitBranch(glslang::TVisit visit, glslang::TIntermBranch*) {}

		int getSymbolId(const glslang::TIntermSymbol* symbol);
		int createVariable(const glslang::TIntermSymbol* node);

		void dump(std::vector<uint8_t>& out) { out.clear(); }

	protected:
		std::vector<ASTVariable> m_vars;

		const glslang::TIntermediate* m_glslang;
		ag::Generator m_gen;

		bool m_isHLSL;

		TGlslangToBVMTraverser(TGlslangToBVMTraverser&);
		TGlslangToBVMTraverser& operator=(TGlslangToBVMTraverser&);
	};
	
	// translate to flags
	uint32_t TranslateBuiltInDecoration(glslang::TBuiltInVariable builtIn, bool memberDeclaration)
	{
		return (int)builtIn;
		/*
		switch (builtIn) {
		    case glslang::EbvPointSize:			return spv::BuiltInPointSize;
			case glslang::EbvPosition:			return spv::BuiltInPosition;
			case glslang::EbvVertexId:			return spv::BuiltInVertexId;
			case glslang::EbvInstanceId:		return spv::BuiltInInstanceId;
			case glslang::EbvVertexIndex:		return spv::BuiltInVertexIndex;
			case glslang::EbvInstanceIndex:		return spv::BuiltInInstanceIndex;
			case glslang::EbvFragCoord:			return spv::BuiltInFragCoord;
			case glslang::EbvPointCoord:		return spv::BuiltInPointCoord;
			case glslang::EbvFace:				return spv::BuiltInFrontFacing;
			case glslang::EbvFragDepth:			return spv::BuiltInFragDepth;
			case glslang::EbvClipDistance:		return spv::BuiltInClipDistance;
			case glslang::EbvCullDistance:		return spv::BuiltInCullDistance;
			case glslang::EbvViewportIndex:		return spv::BuiltInViewportIndex;
			case glslang::EbvSampleId:			return spv::BuiltInSampleId;
			case glslang::EbvSamplePosition:	return spv::BuiltInSamplePosition;
			case glslang::EbvSampleMask:		return spv::BuiltInSampleMask;
			case glslang::EbvLayer:				return spv::BuiltInLayer;
			case glslang::EbvBaseVertex: 		return spv::BuiltInBaseVertex;
			case glslang::EbvBaseInstance: 		return spv::BuiltInBaseInstance;
			case glslang::EbvDrawId: 			return spv::BuiltInDrawIndex;
			case glslang::EbvPrimitiveId: 		return spv::BuiltInPrimitiveId;
			case glslang::EbvFragStencilRef: 	return spv::BuiltInFragStencilRefEXT;
			case glslang::EbvInvocationId:		return spv::BuiltInInvocationId;
			case glslang::EbvTessLevelInner:	return spv::BuiltInTessLevelInner;
			case glslang::EbvTessLevelOuter:	return spv::BuiltInTessLevelOuter;
			case glslang::EbvTessCoord:			return spv::BuiltInTessCoord;
			case glslang::EbvPatchVertices:		return spv::BuiltInPatchVertices;
			case glslang::EbvHelperInvocation:	return spv::BuiltInHelperInvocation;
			case glslang::EbvNumWorkGroups:		return spv::BuiltInNumWorkgroups;
			case glslang::EbvWorkGroupSize:		return spv::BuiltInWorkgroupSize;
			case glslang::EbvWorkGroupId:		return spv::BuiltInWorkgroupId;
			case glslang::EbvLocalInvocationId:	return spv::BuiltInLocalInvocationId;
			case glslang::EbvLocalInvocationIndex:return spv::BuiltInLocalInvocationIndex;
			case glslang::EbvGlobalInvocationId:return spv::BuiltInGlobalInvocationId;
			case glslang::EbvSubGroupSize: 		return spv::BuiltInSubgroupSize;
			case glslang::EbvSubGroupInvocation:return spv::BuiltInSubgroupLocalInvocationId;
			case glslang::EbvNumSubgroups:		return spv::BuiltInNumSubgroups;
			case glslang::EbvSubgroupID:		return spv::BuiltInSubgroupId;

		default:
			return spv::BuiltInMax;
		}
		*/
	}

	std::string convertGlslangToBVMType(const glslang::TType& type)
	{
		switch (type.getBasicType()) {
			case glslang::EbtVoid: return "void";
			case glslang::EbtBool: return "bool";
			case glslang::EbtInt: return "int";
			case glslang::EbtUint: return "uint";
			case glslang::EbtFloat: return "float";
			case glslang::EbtDouble: return "double";
			case glslang::EbtFloat16: return "float16";
			case glslang::EbtInt8: return "int8";
			case glslang::EbtUint8: return "uint8";
			case glslang::EbtInt16: return "int16";
			case glslang::EbtUint16: return "uint16";
			case glslang::EbtInt64: return "int64";
			case glslang::EbtUint64: return "uint64";
			case glslang::EbtAtomicUint: return "atomic_uint";
			case glslang::EbtSampler:
				{
					return "sampler";
					/*
					TODO: samplers
					const glslang::TSampler& sampler = type.getSampler();
					if (sampler.isPureSampler()) {
						spvType = builder.makeSamplerType();
					} else {
						// an image is present, make its type
						spvType = builder.makeImageType(getSampledType(sampler), TranslateDimensionality(sampler),
														sampler.isShadow(), sampler.isArrayed(), sampler.isMultiSample(),
														sampler.isImageClass() ? 2 : 1, TranslateImageFormat(type));
						if (sampler.isCombined()) {
							// already has both image and sampler, make the combined type
							spvType = builder.makeSampledImageType(spvType);
						}
					}
					*/
				}
				break;
			case glslang::EbtStruct:
			case glslang::EbtBlock:
				{
					return "struct";
					/*
					TODO: structures
					// If we've seen this struct type, return it
					const glslang::TTypeList* glslangMembers = type.getStruct();

					// Try to share structs for different layouts, but not yet for other
					// kinds of qualification (primarily not yet including interpolant qualification).
					if (! HasNonLayoutQualifiers(type, qualifier))
						spvType = structMap[explicitLayout][qualifier.layoutMatrix][glslangMembers];
					if (spvType != spv::NoResult)
						break;

					// else, we haven't seen it...
					if (type.getBasicType() == glslang::EbtBlock)
						memberRemapper[glslangMembers].resize(glslangMembers->size());
					spvType = convertGlslangStructToSpvType(type, glslangMembers, explicitLayout, qualifier);
					*/
				}
				break;
			default:
				assert(0);
				break;
		}

		if (type.isMatrix())
			return "matrix"; // TODO: matrices, type.getMatrixCols(), type.getMatrixRows();
		else {
			// If this variable has a vector element count greater than 1, create a SPIR-V vector
			if (type.getVectorSize() > 1)
				return "vector"; // TODO: vectors, type.getVectorSize();
		}

		if (type.isArray()) 
			return "array"; // TODO: arrays
	}

	int TGlslangToBVMTraverser::createVariable(const glslang::TIntermSymbol* node)
	{
		if (node->getQualifier().isConstant()) {
			/*
			spv::Id result = createSpvConstant(*node);
			if (result != spv::NoResult)
				return result;
				*/
			return -1; // TODO: createSpvConstant
		}

		// Now, handle actual variables
		ASTVariable var;
		var.ID = node->getId();
		
		const auto& type = node->getType();

		if (type.getQualifier().isPipeInput())
			var.IsInput = true;
		if (type.getQualifier().isPipeOutput())
			var.IsOutput = true;

		if (type.getQualifier().storage == glslang::EvqUniform) {
			var.IsUniform = true;

			// TODO: constant uniforms
			//if (type.isAtomic())
			//	return spv::StorageClassAtomicCounter;
			//if (type.containsOpaque())
			//	return spv::StorageClassUniformConstant;
		}
		if (type.getQualifier().storage == glslang::EvqUniform)
			var.IsBuffer = true;

		/*
		TODO:
		if (type.getQualifier().isUniformOrBuffer()) {
			if (type.getQualifier().isPushConstant())
				return spv::StorageClassPushConstant;
			if (type.getBasicType() == glslang::EbtBlock)
				return spv::StorageClassUniform;
			return spv::StorageClassUniformConstant;
		}
		*/
	
		switch (type.getQualifier().storage) {
			case glslang::EvqGlobal: var.IsGlobal = true; break;
			case glslang::EvqShared: var.IsShared = true; break;
		}
		
		var.Type = convertGlslangToBVMType(node->getType());

		var.Name = std::string(node->getName().c_str());
		if (glslang::IsAnonymous(var.Name.c_str()))
			var.Name = ""; // TODO: ?

		// TODO: actual bytecode gen

		m_vars.push_back(var);

		return m_vars.size()-1;
	}

	int TGlslangToBVMTraverser::getSymbolId(const glslang::TIntermSymbol* symbol)
	{
		for (int i = 0; i < m_vars.size(); i++)
			if (m_vars[i].ID == symbol->getId())
				return i;

		// it was not found, create it
		uint32_t builtIn = TranslateBuiltInDecoration(symbol->getQualifier().builtIn, false);
		int idRet = createVariable(symbol);
		
		if (symbol->getQualifier().hasLocation()) {} // TODO: symbol->getQualifier().layoutLocation
		if (symbol->getQualifier().hasSet()) {} // TODO: symbol->getQualifier().layoutSet
		if (symbol->getQualifier().hasBinding()) {} // symbol->getQualifier().layoutBinding
		if (symbol->getQualifier().hasAttachment()) {} // TODO: symbol->getQualifier().layoutAttachment
		
		// add built-in variable decoration
		if (builtIn != glslang::TBuiltInVariable::EbvLast &&
			builtIn != glslang::TBuiltInVariable::EbvNone) { } // TODO: built in variables

		if (symbol->getType().isImage()) { } // TODO: image

		// nonuniform
		// TODO? builder.addDecoration(id, TranslateNonUniformDecoration(symbol->getType().getQualifier()));

		if (symbol->getType().getQualifier().semanticName != nullptr) {
			// TODO: symbol->getType().getQualifier().semanticName
		}

		if (symbol->isReference()) {} // TODO: reference

		return idRet;
	}

	void TGlslangToBVMTraverser::visitSymbol(glslang::TIntermSymbol* symbol)
	{
		printf("var: %s\n", symbol->getName().c_str());

		// set up all the IO decorations on the first call
		int id = getSymbolId(symbol);
	}

	void BVMGenerator::Translate(const glslang::TIntermediate& intermediate, std::vector<uint8_t>& out)
	{
		TIntermNode* root = intermediate.getTreeRoot();

		if (root == 0)
			return;

		glslang::GetThreadPoolAllocator().push();

		TGlslangToBVMTraverser it(&intermediate);
		it.init();
		root->traverse(&it);
		it.dump(out);

		glslang::GetThreadPoolAllocator().pop();
	}
}